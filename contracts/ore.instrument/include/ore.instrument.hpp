#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include "eosiolib/eosio.hpp"
#include "eosiolib/asset.hpp"
#include "eosiolib/print.hpp"
#include "eosiolib/transaction.hpp"
#include "eosiolib/time.hpp"
#include "ore.rights_registry.hpp"

using namespace eosio;
using namespace std;

class [[eosio::contract("ore.instrument")]] instrument : public eosio::contract
{
  public:
    instrument( name receiver, name code, datastream<const char*> ds): contract(receiver, code, ds), _account(receiver, receiver.value), _tokens(receiver, receiver.value) {}

    struct instrument_data
    {
        name issuer;
        string instrument_class;
        string description;
        string instrument_template;
        string security_type;
        vector<oretypes::param_type> parameter_rules;
        vector<oretypes::right> rights;
        uint64_t parent_instrument_id;

        //example input for data: considerations
        vector<oretypes::args> data;
        string encrypted_by;
        uint8_t mutability; // 0- immutable, 1- only datesi 2- all
    };

    TABLE token
    {
        //721 standard properties
        uint64_t id;
        name owner;
        name minted_by;
        uint64_t minted_at;

        //instrument properties
        instrument_data instrument;
        bool revoked;
        uint64_t start_time;
        uint64_t end_time;

        uint64_t template_hash;
        uint64_t class_hash;

        uint64_t primary_key() const { return id; }
        uint64_t by_owner() const { return owner.value; }
        uint64_t by_template() const { return template_hash; }
        uint64_t by_class() const { return class_hash; }

        EOSLIB_SERIALIZE(token, (id)(owner)(minted_by)(minted_at)(instrument)(revoked)(start_time)(end_time)(template_hash)(class_hash))
    };

    typedef eosio::multi_index<"tokens"_n, token,
                       indexed_by<"owner"_n, const_mem_fun<token, uint64_t, &token::by_owner>>,
                       indexed_by<"templatehash"_n, const_mem_fun<token, uint64_t, &token::by_template>>,
                       indexed_by<"classhash"_n, const_mem_fun<token, uint64_t, &token::by_class>>
            >tokenindex;

    tokenindex _tokens;
    
    TABLE accountdata
    {
        name owner;
        uint64_t balance;
        vector<uint64_t> instruments;
        uint64_t primary_key() const { return owner.value; }
        EOSLIB_SERIALIZE(accountdata, (owner)(balance)(instruments))
    };

    typedef eosio::multi_index<"account"_n, accountdata> accountindex;

    accountindex _account;

  private:
    TABLE accountbalance
    {
        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };

    TABLE currencystat
    {
        asset supply;
        asset max_supply;
        name issuer;

        uint64_t primary_key() const { return supply.symbol.code().raw(); }
    };

    typedef eosio::multi_index<"accounts"_n, accountbalance> accounts;
    typedef eosio::multi_index<"stat"_n, currencystat> stats;

    void sub_balance(name owner, asset value);
    void sub_balance_from(name sender, name owner, asset value);
    void add_balance(name owner, asset value, name ram_payer);
    void transfer_balances(name from, name to, uint64_t instrument_id, int64_t amount = 1);

  public:
    //public utility functions
    token find_token_by_id(uint64_t id);
    bool isToken(uint64_t id);
    token find_token_by_template(string instrument_template);
    bool _owns(name claimant, uint64_t token_id);
    uint64_t total_supply();
    uint64_t balance_of(name owner);
    name owner_of(uint64_t token_id);

    inline static uint64_t hashStringToInt(const string &strkey)
    {
        return hash<string>{}(strkey);
    }

    //actions
    ACTION mint(name minter, name owner, instrument_data instrument, uint64_t start_time, uint64_t end_time, uint64_t instrumentId);
    ACTION checkright(name minter, name issuer, string rightname, uint64_t deferred_transaction_id);
    ACTION update(name updater, string instrument_template, instrument_data instrument, uint64_t instrument_id, uint64_t start_time, uint64_t end_time);
    ACTION transfer(name sender, name to, uint64_t token_id);
    ACTION revoke(name revoker, uint64_t token_id);
    ACTION burn(name burner, uint64_t token_id);
    ACTION create(name issuer, asset maximum_supply);
    ACTION createinst(name minter, name owner, uint64_t instrumentId, instrument_data instrument, uint64_t start_time, uint64_t end_time);
    ACTION issue(name to, asset quantity, string memo);
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
    auto hashtable = _tokens.get_index<"templatehash"_n>();
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
uint64_t instrument::balance_of(name owner)
{
    auto account = _account.find(owner.value);
    return account->balance;
}

// Returns who owns a token
name instrument::owner_of(uint64_t token_id)
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
bool instrument::_owns(name claimant, uint64_t token_id)
{
    return owner_of(token_id) == claimant;
}

void instrument::transfer_balances(name from, name to, uint64_t instrument_id, int64_t amount)
{
    auto fromitr = _account.find(from.value);

    eosio_assert(fromitr != _account.end(), "Sender account doesn't exists");
    eosio_assert(fromitr->balance > 0, "Sender account's balance is 0");

    _account.modify(fromitr, same_payer, [&](auto &a) {
        a.balance -= amount;
        a.instruments.erase(std::remove(a.instruments.begin(), a.instruments.end(), instrument_id), a.instruments.end());
    });

    auto toitr = _account.find(to.value);

    if (toitr != _account.end())
    {
        _account.modify(toitr, same_payer, [&](auto &a) {
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