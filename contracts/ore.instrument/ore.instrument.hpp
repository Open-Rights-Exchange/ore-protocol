#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include "../ore.rights_registry/ore.rights_registry.hpp"

using namespace eosio;
using namespace std;

class instrument : public eosio::contract
{
  public:
    instrument(account_name self)
        : contract(self), _account(_self, _self), _tokens(_self, _self) {}

    struct instrument_data
    {
        account_name issuer;
        string instrument_class;
        string description;
        string instrument_template;
        string security_type;
        vector<ore_types::param_type> parameter_rules;
        vector<ore_types::right> rights;
        uint64_t parent_instrument_id;

        //example input for data: considerations
        vector<ore_types::args> data;
        string encrypted_by;
        uint8_t mutability; // 0- immutable, 1- only datesi 2- all
    };

    //@abi table tokens i64
    struct token
    {
        //721 standard properties
        uint64_t id;
        account_name owner;
        account_name minted_by;
        uint64_t minted_at;

        //instrument properties
        instrument_data instrument;
        bool revoked;
        uint64_t start_time;
        uint64_t end_time;

        uint64_t template_hash;
        uint64_t class_hash;

        uint64_t primary_key() const { return id; }
        uint64_t by_owner() const { return owner; }
        uint64_t by_template() const { return template_hash; }
        uint64_t by_class() const { return class_hash; }

        EOSLIB_SERIALIZE(token, (id)(owner)(minted_by)(minted_at)(instrument)(revoked)(start_time)(end_time)(template_hash)(class_hash))
    };

    eosio::multi_index<N(tokens), token,
                       indexed_by<N(owner), const_mem_fun<token, uint64_t, &token::by_owner>>,
                       indexed_by<N(template_hash), const_mem_fun<token, uint64_t, &token::by_template>>,
                       indexed_by<N(class_hash), const_mem_fun<token, uint64_t, &token::by_class>>>
        _tokens;

    //@abi table account i64
    struct accountdata
    {
        account_name owner;
        uint64_t balance;
        vector<uint64_t> instruments;
        uint64_t primary_key() const { return owner; }
        EOSLIB_SERIALIZE(accountdata, (owner)(balance)(instruments))
    };

    eosio::multi_index<N(account), accountdata> _account;

    // NOTE: Uncomment and use in future if required
    // Schema for allowance feature like in ERC-721
    //@abi table allowances i64
    // struct allowancedata
    // {
    //     uint64_t token_id;
    //     account_name to;

    //     uint64_t primary_key() const { return token_id; }

    //     EOSLIB_SERIALIZE(allowancedata, (token_id)(to))
    // };

    // typedef eosio::multi_index<N(allowances), allowancedata> allowances;

  private:
    //@abi table accounts i64
    struct account
    {
        asset balance;

        uint64_t primary_key() const { return balance.symbol.name(); }
    };

    //@abi table stat i64
    struct currencystat
    {
        asset supply;
        asset max_supply;
        account_name issuer;

        uint64_t primary_key() const { return supply.symbol.name(); }
    };

    typedef eosio::multi_index<N(accounts), account> accounts;
    typedef eosio::multi_index<N(stat), currencystat> stats;

    void sub_balance(account_name owner, asset value);
    void sub_balance_from(account_name sender, account_name owner, asset value);
    void add_balance(account_name owner, asset value, account_name ram_payer);
    void transfer_balances(account_name from, account_name to, uint64_t instrument_id, int64_t amount = 1);

  public:
    //public utility functions
    token find_token_by_id(uint64_t id);
    bool isToken(uint64_t id);
    token find_token_by_template(string instrument_template);
    bool _owns(account_name claimant, uint64_t token_id);
    uint64_t total_supply();
    uint64_t balance_of(account_name owner);
    account_name owner_of(uint64_t token_id);

    inline static uint64_t hashStringToInt(const string &strkey)
    {
        return hash<string>{}(strkey);
    }

    //actions
    void approve(account_name from, account_name to, uint64_t token_id);
    void mint(account_name minter, account_name owner, instrument_data instrument, uint64_t start_time, uint64_t end_time, uint64_t instrumentId);
    void checkright(account_name minter, account_name issuer, string rightname, uint64_t deferred_transaction_id);
    void update(account_name updater, string instrument_template, instrument_data instrument, uint64_t instrument_id, uint64_t start_time, uint64_t end_time);
    void transfer(account_name sender, account_name to, uint64_t token_id);
    void revoke(account_name revoker, uint64_t token_id);
    void burn(account_name burner, uint64_t token_id);
    void create(account_name issuer, asset maximum_supply);
    void createinst(account_name minter, account_name owner, uint64_t instrumentId, instrument_data instrument, uint64_t start_time, uint64_t end_time);
    void updateinst(account_name updater, account_name owner, uint64_t instrumentId, instrument_data instrument, uint64_t start_time, uint64_t end_time);
    void issue(account_name to, asset quantity, string memo);
};

instrument::token instrument::find_token_by_id(uint64_t id)
{
    auto tok = _tokens.find(id);

    if (tok == _tokens.end())
        eosio_assert(false, "token not found");

    return token{
        tok->id,
        tok->owner,
        tok->minted_by,
        tok->minted_at,
        tok->instrument,
        tok->revoked,
        tok->start_time,
        tok->end_time,
        tok->template_hash,
        tok->class_hash};
}

bool instrument::isToken(uint64_t id)
{
    auto tok = _tokens.find(id);

    if (tok == _tokens.end())
        return false;
    else
        return true;
}

instrument::token instrument::find_token_by_template(string instrument_template)
{
    auto hashtable = _tokens.get_index<N(template_hash)>();
    auto item = hashtable.find(hashStringToInt(instrument_template));
    if (item == hashtable.end())
        eosio_assert(false, "instrument with given template not found");
    return {item->id,
            item->owner,
            item->minted_by,
            item->minted_at,
            item->instrument,
            item->revoked,
            item->start_time,
            item->end_time,
            item->template_hash,
            item->class_hash};
}

// Return an account's total balance
uint64_t instrument::balance_of(account_name owner)
{
    auto account = _account.find(owner);
    return account->balance;
}

// Returns who owns a token
account_name instrument::owner_of(uint64_t token_id)
{
    auto token = _tokens.find(token_id);
    return token->owner;
}

uint64_t instrument::total_supply()
{
    auto tokitr = _tokens.begin();
    uint64_t token_id = 0;
    while (tokitr != _tokens.end())
    {
        token_id++;
        tokitr++;
    }

    return token_id;
}

// Check if account owns the token
bool instrument::_owns(account_name claimant, uint64_t token_id)
{
    return owner_of(token_id) == claimant;
}

void instrument::transfer_balances(account_name from, account_name to, uint64_t instrument_id, int64_t amount)
{
    auto fromitr = _account.find(from);

    eosio_assert(fromitr != _account.end(), "Sender account doesn't exists");
    eosio_assert(fromitr->balance > 0, "Sender account's balance is 0");

    _account.modify(fromitr, 0, [&](auto &a) {
        a.balance -= amount;
        a.instruments.erase(std::remove(a.instruments.begin(), a.instruments.end(), instrument_id), a.instruments.end());
    });

    auto toitr = _account.find(to);

    if (toitr != _account.end())
    {
        _account.modify(toitr, 0, [&](auto &a) {
            a.balance += amount;
            a.instruments.push_back(instrument_id);
        });
    }
    else
    {
        _account.emplace(from, [&](auto &a) {
            a.owner = to;
            a.balance = amount;
            a.instruments.push_back(instrument_id);
        });
    }
}