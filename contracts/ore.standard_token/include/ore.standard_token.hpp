/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include "eosiolib/asset.hpp"
#include "eosiolib/eosio.hpp"

#include <string>

namespace eosiosystem
{
class system_contract;
}

namespace eosio
{

using std::string;

class[[eosio::contract("ore.standard_token")]] oretoken : public contract
{
  public:
    using contract::contract;

    ACTION create(name issuer, asset maximum_supply);

    ACTION issue(name to, asset quantity, string memo);

    ACTION retire(asset quantity, string memo);

    ACTION transfer(name from, name to, asset quantity, string memo);

    ACTION approve(name from, name to, asset quantity, string memo);

    ACTION transferfrom(name sender, name from, name to, asset quantity, string memo);

    ACTION open(name owner, const symbol &symbol, name ram_payer);

    ACTION close(name owner, const symbol &symbol);

    static asset get_supply( name token_contract_account, symbol_code sym_code )
    {
        stats statstable( token_contract_account, sym_code.raw() );
        const auto& st = statstable.get( sym_code.raw() );
        return st.supply;
    }

    static asset get_balance( name token_contract_account, name owner, symbol_code sym_code )
    {
        accounts accountstable( token_contract_account, owner.value );
        const auto& ac = accountstable.get( sym_code.raw() );
        return ac.balance;
    }

    inline asset allowance_of(name from, name to);

  private:
    inline void set_allowance(name from, name to, asset quantity, bool increment = false);

    TABLE account
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

    TABLE allowance
    {
        name to;
        asset quantity;

        uint64_t primary_key() const { return to.value; }

        EOSLIB_SERIALIZE(allowance, (to)(quantity))
    };

    typedef eosio::multi_index<"allowances"_n, allowance> _allowances;
    typedef eosio::multi_index<"accounts"_n, account> accounts;
    typedef eosio::multi_index<"stat"_n, currencystat> stats;

    void sub_balance(name owner, asset value);
    void sub_balance_from(name sender, name owner, asset value);
    void add_balance(name owner, asset value, name ram_payer);

  public:
    struct transfer_args
    {
        name from;
        name to;
        asset quantity;
        string memo;
    };
};

void oretoken::set_allowance(name from, name to, asset quantity, bool increment)
{
    auto sym = quantity.symbol.code();
    stats statstable(_self, sym.raw());
    const auto &st = statstable.get(sym.raw());

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount >= 0, "must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

    name key = to;

    _allowances allowances(_self, from.value);

    auto allowanceitr = allowances.find(key.value);

    if (allowanceitr == allowances.end())
    {
        allowances.emplace(from, [&](auto &a) {
            a.to = to;
            a.quantity = quantity;
        });
    }
    else
    {
        allowances.modify(allowanceitr, same_payer, [&](auto &a) {
            if (!increment)
                a.quantity = quantity;
            else
                a.quantity += quantity;
        });
    }
}

asset oretoken::allowance_of(name from, name to)
{
    _allowances allowances(_self, from.value);

    auto allowanceitr = allowances.find(to.value);

    eosio_assert(allowanceitr != allowances.end(), "no allowance approved for this account");

    return allowanceitr->quantity;
}

} // namespace eosio