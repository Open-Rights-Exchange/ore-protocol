/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem
{
class system_contract;
}

namespace eosio
{

using std::string;

class token : public contract
{
  public:
    token(account_name self) : contract(self) {}

    void create(account_name issuer,
                asset maximum_supply);

    void issue(account_name to, asset quantity, string memo);

    void retire(asset quantity, string memo);

    void transfer(account_name from,
                  account_name to,
                  asset quantity,
                  string memo);

    void approve(account_name from, account_name to, asset quantity, string memo);

    void transferfrom(account_name sender, account_name from, account_name to, asset quantity, string memo);

    inline asset get_supply(symbol_name sym) const;

    inline asset get_balance(account_name owner, symbol_name sym) const;

    inline asset allowance_of(account_name from, account_name to);

  private:
    inline void set_allowance(account_name from, account_name to, asset quantity, bool increment = false);

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

    //@abi table allowances i64
    struct allowance
    {
        account_name to;
        asset quantity;

        uint64_t primary_key() const { return to; }

        EOSLIB_SERIALIZE(allowance, (to)(quantity))
    };

    typedef eosio::multi_index<N(allowances), allowance> _allowances;
    typedef eosio::multi_index<N(accounts), account> accounts;
    typedef eosio::multi_index<N(stat), currencystat> stats;

    void sub_balance(account_name owner, asset value);
    void sub_balance_from(account_name sender, account_name owner, asset value);
    void add_balance(account_name owner, asset value, account_name ram_payer);

  public:
    struct transfer_args
    {
        account_name from;
        account_name to;
        asset quantity;
        string memo;
    };
};

asset token::get_supply(symbol_name sym) const
{
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);
    return st.supply;
}

asset token::get_balance(account_name owner, symbol_name sym) const
{
    accounts accountstable(_self, owner);
    const auto &ac = accountstable.get(sym);
    return ac.balance;
}

void token::set_allowance(account_name from, account_name to, asset quantity, bool increment)
{
    auto sym = quantity.symbol.name();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount >= 0, "must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

    account_name key = to;

    _allowances allowances(_self, from);

    auto allowanceitr = allowances.find(key);

    if (allowanceitr == allowances.end())
    {
        allowances.emplace(from, [&](auto &a) {
            a.to = to;
            a.quantity = quantity;
        });
    }
    else
    {
        allowances.modify(allowanceitr, 0, [&](auto &a) {
            if (!increment)
                a.quantity = quantity;
            else
                a.quantity += quantity;
        });
    }
}

asset token::allowance_of(account_name from, account_name to)
{
    _allowances allowances(_self, from);

    auto allowanceitr = allowances.find(to);

    eosio_assert(allowanceitr != allowances.end(), "no allowance approved for this account");

    return allowanceitr->quantity;
}

} // namespace eosio