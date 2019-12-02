/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

/**
 *  Following contract adheres to eosio.token standards. 
 * It copies all the functionalities of eosio.token. In addition to that, it implements functions for allowance model as in ERC-20
 */

/**
 * NOTE: Any changes to create, issue, transfer, sub_balance, sub_balance_from and add_balance functions should be replicated 
 * in the implementation of the same functions in ore.instrument contract.
 */

#include "../include/ore.standard_token.hpp"

namespace eosio
{

ACTION oretoken::create(name issuer,
                        asset maximum_supply)
{
    require_auth(_self);

    auto sym = maximum_supply.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "invalid supply");
    eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    eosio_assert(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
}

// The approve action is called by "from" account to authorize "to" account to call the transferfrom function on it's behalf
ACTION oretoken::approve(name from, name to, asset quantity, string memo)
{
    require_auth(from);

    accounts from_acnts(_self, from.value);

    const auto &from_account = from_acnts.get(quantity.symbol.code().raw(), "no balance object found");
    eosio_assert(from_account.balance.amount >= quantity.amount, "amount being approved is more than the balance of the approver account");

    set_allowance(from, to, quantity, true);
}

ACTION oretoken::issue(name to, asset quantity, string memo)
{
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
    const auto &st = *existing;

    require_auth(st.issuer);
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must issue positive quantity");

    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify(st, same_payer, [&](auto &s) {
        s.supply += quantity;
    });

    add_balance(st.issuer, quantity, st.issuer);

    if (to != st.issuer)
    {
        SEND_INLINE_ACTION(*this, transfer, {st.issuer, "active"_n}, {st.issuer, to, quantity, memo});
    }
} // namespace eosio

ACTION oretoken::retire(asset quantity, string memo)
{
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    eosio_assert(existing != statstable.end(), "token with symbol does not exist");
    const auto &st = *existing;

    require_auth(st.issuer);
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must retire positive quantity");

    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

    statstable.modify(st, same_payer, [&](auto &s) {
        s.supply -= quantity;
    });

    sub_balance(st.issuer, quantity);
}

ACTION oretoken::transfer(name from,
                          name to,
                          asset quantity,
                          string memo)
{
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);
    eosio_assert(is_account(to), "to account does not exist");
    auto sym = quantity.symbol.code();
    stats statstable(_self, sym.raw());
    const auto &st = statstable.get(sym.raw());

    require_recipient(from);
    require_recipient(to);

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    auto payer = has_auth(to) ? to : from;

    sub_balance(from, quantity);
    add_balance(to, quantity, payer);
}

// This action is called by the approved "sender" account on behalf of the "from" account to transfer "quantity" to "to" account
ACTION oretoken::transferfrom(name sender, name from, name to, asset quantity, string memo)
{
    require_auth(sender);

    auto allowance = oretoken::allowance_of(from, sender);

    print("The approved account ", name{sender});
    print(" is transferring ", quantity);
    print(" from ", name{from});
    print(" to ", name{to});

    eosio_assert(allowance.amount >= quantity.amount, "the amount being transferred is more than the approved amount");

    add_balance(to, quantity, sender);
    sub_balance_from(sender, from, quantity);

    set_allowance(from, sender, allowance - quantity, false);
}

// eosio.token standard sub_balance function
void oretoken::sub_balance(name owner, asset value)
{
    accounts from_acnts(_self, owner.value);

    const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
    eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

    {
        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }
}

// It is used by transfer_from account to specify the RAM payer as the "sender" account and not the "owner" account as in the sub_balance function
void oretoken::sub_balance_from(name sender, name owner, asset value)
{
    accounts from_acnts(_self, owner.value);

    const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
    eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

    from_acnts.modify(from, sender, [&](auto &a) {
        a.balance -= value;
    });
}

void oretoken::add_balance(name owner, asset value, name ram_payer)
{
    accounts to_acnts(_self, owner.value);
    auto to = to_acnts.find(value.symbol.code().raw());
    if (to == to_acnts.end())
    {
        to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
        });
    }
    else
    {
        to_acnts.modify(to, same_payer, [&](auto &a) {
            a.balance += value;
        });
    }
}

ACTION oretoken::open(name owner, const symbol &symbol, name ram_payer)
{
    require_auth(ram_payer);

    auto sym_code_raw = symbol.code().raw();

    stats statstable(_self, sym_code_raw);
    const auto &st = statstable.get(sym_code_raw, "symbol does not exist");
    eosio_assert(st.supply.symbol == symbol, "symbol precision mismatch");

    accounts acnts(_self, owner.value);
    auto it = acnts.find(sym_code_raw);
    if (it == acnts.end())
    {
        acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = asset{0, symbol};
        });
    }
}

ACTION oretoken::close(name owner, const symbol &symbol)
{
    require_auth(owner);
    accounts acnts(_self, owner.value);
    auto it = acnts.find(symbol.code().raw());
    eosio_assert(it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect.");
    eosio_assert(it->balance.amount == 0, "Cannot close because the balance is not zero.");
    acnts.erase(it);
}

} // namespace eosio

EOSIO_DISPATCH(eosio::oretoken, (create)(issue)(transfer)(approve)(transferfrom)(open)(close)(retire))