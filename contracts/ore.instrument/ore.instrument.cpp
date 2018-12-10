#include "ore.instrument.hpp"
using namespace eosio;

// transaction id for deferred transaction
account_name RIGHTS_CONTRACT_NAME = N(rights.ore);

// Creates new instrument
// NOTE: this should result in changes in the following tables :
// tokens   - add new token
// account  - instrument owner's list of owned instruments get updated
// accounts - OREINST symbol balance gets updated

// mint action internally calls a deferred transaction with 2 types of actions:
// checkright  - for each right in the instrument object, mint instrument calls check right within a deferred transaction
// createinst  - once all the rights are checked, the last action in the deferred transaction is createinst which adds the instrument to the tokens table
// NOTE: if any of the checkright action fails, it will cancel the deferred transaction and the creatinst action will not be called. Hence no instrument will be created.
void instrument::mint(account_name minter, account_name owner, instrument_data instrument,
                      uint64_t start_time, uint64_t end_time, uint64_t instrumentId = 0)
{
    // Checking if the minter has the required authorization
    require_auth(minter);
    auto hashtable = _tokens.get_index<N(template_hash)>();
    auto item = hashtable.find(hashStringToInt(instrument.instrument_template));

    if (instrument.rights.size() == 0)
    {
        eosio_assert(false, "Right array is empty");
    }

    // If instrumentId value passed as 0, next available primate key will be automatically assigned as instrumentId
    // This mean, you can not set instrumentId specifically to 0, because it means pick the next available key.
    if (instrumentId == 0)
    {
        if (_tokens.available_primary_key() == 0)
        {
            instrumentId = 1; // assigning the available key
        }
        else
        {
            instrumentId = _tokens.available_primary_key();
        }
    }
    else
    {
        auto institr = _tokens.find(instrumentId);
        // If an id value specified (other than 0) and the id already exists in the table, assertion will be thrown
        eosio_assert(institr == _tokens.end(), "instrumentId exists!");
    }

    // If an instrument already exists with the given template name, get the instrument data from the existing instrument
    if (instrument.instrument_template != "" && item != hashtable.end())
    {
        instrument.issuer = item->instrument.issuer;
        instrument.instrument_class = item->instrument.instrument_class;
        instrument.description = item->instrument.description;
        instrument.security_type = item->instrument.security_type;
        instrument.parameter_rules = item->instrument.parameter_rules;
        instrument.rights = item->instrument.rights;
        instrument.parent_instrument_id = item->instrument.parent_instrument_id;
        instrument.data = item->instrument.data;
        instrument.encrypted_by = item->instrument.encrypted_by;
        instrument.mutability = item->instrument.mutability;

        // create a deferred transaction object to add instrument to the tokens table
        transaction create_instrument{};

        // create an unique id for the deferred transaction
        uint64_t create_transaction_id = instrumentId;

        // Adding createinst action to the deferred transaction to add the new instrument to the tokens table
        create_instrument.actions.emplace_back(
            permission_level{N(instr.ore), N(active)}, _self, N(createinst),
            std::make_tuple(minter,
                            owner,
                            instrumentId,
                            instrument,
                            start_time,
                            end_time,
                            create_transaction_id));

        // send deferred transaction
        create_instrument.send(instrumentId, minter);
    }
    else
    {
        // create a deferred transaction object to check rights and add instrument to the tokens table
        transaction deferred_instrument{};

        // create an unique id for the deferred transaction
        uint64_t deferred_trx_id = now();

        // checking if the issuer is the owner of the rights
        for (int i = 0; i < instrument.rights.size(); i++)
        {
            deferred_instrument.actions.emplace_back(
                permission_level{N(instr.ore), N(active)}, _self, N(checkright),
                std::make_tuple(
                    minter,
                    instrument.issuer,
                    instrument.rights[i].right_name,
                    deferred_trx_id));
        }

        // Adding createinst action to the deferred transaction to add the new instrument to the tokens table
        deferred_instrument.actions.emplace_back(
            permission_level{N(instr.ore), N(active)}, _self, N(createinst),
            std::make_tuple(minter,
                            owner,
                            instrumentId,
                            instrument,
                            start_time,
                            end_time));

        // send deferred transaction
        deferred_instrument.send(instrumentId, minter);
    }
}

void instrument::createinst(account_name minter, account_name owner, uint64_t instrumentId, instrument_data instrument, uint64_t start_time, uint64_t end_time)
{
    require_auth(_self);
    auto accountitr = _account.find(owner);

    // check if account is already registered to accounts table
    if (accountitr == _account.end())
    {
        _account.emplace(_self, [&](auto &a) {
            a.owner = owner;
            a.balance = 0;
            print("new instrument account: ", a.primary_key(), "\n");
        });
        accountitr = _account.find(owner);
    }

    // writing to tokens table
    _tokens.emplace(_self, [&](auto &a) {
        a.id = instrumentId;
        a.owner = owner;
        a.minted_by = minter;
        a.minted_at = time(0);
        a.instrument = instrument;
        a.revoked = false;
        a.start_time = start_time;
        a.end_time = end_time;
        a.template_hash = hashStringToInt(instrument.instrument_template);
        a.class_hash = hashStringToInt(instrument.instrument_class);
    });

    // increasing the account balance (total token count)
    _account.modify(accountitr, 0, [&](auto &a) {
        a.balance++;
        a.instruments.push_back(instrumentId);
    });

    print("minter", name{minter});

    print("action:mint instrument:", instrumentId, " to:", name{owner}, "\n");

    eosio_assert(is_account(owner), "to account does not exist");

    // transfer 1 OREINST from the issuer account for OREINST to the owner account of instrument
    sub_balance(_self, asset(10000, symbol_type(S(4, OREINST))));
    add_balance(owner, asset(10000, symbol_type(S(4, OREINST))), _self);
}

void instrument::updateinst(account_name updater, account_name owner, uint64_t instrumentId, instrument_data instrument, uint64_t start_time, uint64_t end_time)
{
    require_auth(_self);
    auto accountitr = _account.find(owner);

    auto tokenitr = _tokens.find(instrumentId);

    // writing to tokens table
    _tokens.modify(tokenitr, 0, [&](auto &a) {
        a.id = instrumentId;
        a.owner = owner;
        a.minted_by = a.minted_by;
        a.minted_at = time(0);
        a.instrument = instrument;
        a.revoked = false;
        a.start_time = start_time;
        a.end_time = end_time;
        a.template_hash = hashStringToInt(instrument.instrument_template);
        a.class_hash = hashStringToInt(instrument.instrument_class);
    });

    print("minter", name{updater});

    print("action:update instrument:", instrumentId, " to:", name{owner}, "\n");

    eosio_assert(is_account(owner), "to account does not exist");
}

void instrument::checkright(account_name minter, account_name issuer, string rightname, uint64_t deferred_transaction_id = 0)
{

    require_auth(_self);

    //instantiating rights.ore contract
    rights_registry rights_contract = rights_registry(RIGHTS_CONTRACT_NAME);

    print("action:checkright:", rightname, "\n");

    auto rightitr = rights_contract.find_right_by_name(rightname);
    if (rightitr.owner == 0)
    {
        if (deferred_transaction_id != 0)
        {
            cancel_deferred(deferred_transaction_id);
        }
        eosio_assert(false, "right doesn't exist");
    }

    // check if the minter of the instrument is the issuer of the right
    bool minter_owns_right = rightitr.owner == minter;
    if (!minter_owns_right)
    {
        auto position_in_whitelist = std::find(rightitr.issuer_whitelist.begin(), rightitr.issuer_whitelist.end(), minter);
        eosio_assert(position_in_whitelist != rightitr.issuer_whitelist.end(), "minter neither owns the right nor whitelisted for the right");
    }

    bool issuer_owns_right = rightitr.owner == issuer;

    if (!issuer_owns_right)
    {
        auto issuer_in_whitelist = std::find(rightitr.issuer_whitelist.begin(), rightitr.issuer_whitelist.end(), issuer);
        if (issuer_in_whitelist == rightitr.issuer_whitelist.end())
        {
            if (deferred_transaction_id != 0)
            {
                cancel_deferred(deferred_transaction_id);
            }
            eosio_assert(true, "instrument issuer neither holds the right nor whitelisted for the right");
        }
    }
}

// updates an instrument
// NOTE: this should result in changes in the following table :
// tokens  - the instrument token gets updated depending on the mutabiility
// if mutability is 1, start_time and/or end_time can be updated
// if mutability is 2, everything except the owner can be updated
void instrument::update(account_name updater, string instrument_template, instrument_data instrument = {},
                        uint64_t instrument_id = 0, uint64_t start_time = 0, uint64_t end_time = 0)
{
    require_auth(updater);
    uint64_t new_start;
    uint64_t new_end;

    instrument::token item;
    if (instrument_id != 0)
    {
        item = find_token_by_id(instrument_id);
    }
    else
    {
        item = find_token_by_template(instrument_template);
    }

    eosio_assert(item.owner == updater || item.minted_by == updater, "updater acccount doesn't have the authority to change start/emd time of the instrument");

    eosio_assert(item.revoked == false, "Token is already revoked");

    eosio_assert(item.instrument.mutability == 1 || item.instrument.mutability == 2, "the instrument to be updated is immutable");

    rights_registry rights_contract = rights_registry(RIGHTS_CONTRACT_NAME);

    auto tokenitr = _tokens.find(item.id);

    // Update start time/ end time for mutability 1 and 2
    // No updates if the input start time/end time is 0
    if (start_time != 0)
    {
        new_start = start_time;
    }
    else
    {
        new_start = item.start_time;
    }

    if (end_time != 0)
    {
        new_end = end_time;
    }
    else
    {
        new_end = item.end_time;
    }

    if (item.instrument.mutability == 1)
    {
        // update the instrument token in the tokens table
        _tokens.modify(tokenitr, 0, [&](auto &a) {
            a.start_time = new_start;
            a.end_time = new_end;
        });
    }
    else
    {
        item.instrument.issuer = instrument.issuer;
        item.instrument.instrument_class = instrument.instrument_class;
        item.instrument.description = instrument.description;
        item.instrument.security_type = instrument.security_type;
        item.instrument.parameter_rules = instrument.parameter_rules;
        item.instrument.rights = instrument.rights;
        item.instrument.parent_instrument_id = instrument.parent_instrument_id;
        item.instrument.data = instrument.data;
        item.instrument.encrypted_by = instrument.encrypted_by;
        item.instrument.mutability = instrument.mutability;

        transaction deferred_instrument{};

        uint64_t deferred_trx_id = instrument_id;

        for (int i = 0; i < item.instrument.rights.size(); i++)
        {
            deferred_instrument.actions.emplace_back(
                permission_level{N(instr.ore), N(active)}, _self, N(checkright),
                std::make_tuple(
                    updater,
                    instrument.issuer,
                    instrument.rights[i].right_name,
                    deferred_trx_id));
        }

        // Adding createinst action to the deferred transaction to add the new instrument to the tokens table
        deferred_instrument.actions.emplace_back(
            permission_level{N(instr.ore), N(active)}, _self, N(updateinst),
            std::make_tuple(updater,
                            item.owner,
                            instrument_id,
                            instrument,
                            start_time,
                            end_time));

        // send deferred transaction
        deferred_instrument.send(deferred_trx_id, updater);
    }

    print("updater", name{updater});

    print("action:update instrument:", instrument_id, "\n");
}

// transfers an instrument
// NOTE: this should result in changes in the following tables :
// tokens   - owner field of the token gets updated
// account  - instrument owner's list of owned instruments get updated
// accounts - OREINST symbol balance gets updated
void instrument::transfer(account_name sender, account_name to, uint64_t token_id)
{
    require_auth(sender);

    //find token
    auto tokenitr = _tokens.find(token_id);

    eosio_assert(tokenitr->owner == sender, "Sender account is not allowed to transfer the instrument");

    eosio_assert(tokenitr->revoked == false, "token is revoked");

    eosio_assert(tokenitr != _tokens.end(), "Token doesn't exists");

    // NOTE: Use in future if required
    // Allows an "allowed" account from the allowance table to be able to do transfer on the instrument owner's behalf
    // bool is_approved_sender = false;
    // if (tokenitr->owner != sender)
    // {
    //     allowances _allowances(_self, tokenitr->owner);
    //     auto allowanceitr = _allowances.find(token_id);
    //     eosio_assert(allowanceitr->to == sender, "Sender is not allowed");
    //     _allowances.erase(allowanceitr);
    //     is_approved_sender = true;
    // }
    // increment/decrement balances
    // if (is_approved_sender)
    // {
    //     transfer_balances(sender, tokenitr->owner, to, token_id);
    //     sub_balance_from(sender, tokenitr->owner, asset(10000, symbol_type(S(4, OREINST))));
    //     add_balance(to, asset(10000, symbol_type(S(4, OREINST))), sender);
    // }
    // else
    // {
    //     transfer_balances(sender, sender, to, token_id);
    //     sub_balance(sender, asset(10000, symbol_type(S(4, OREINST))));
    //     add_balance(to, asset(10000, symbol_type(S(4, OREINST))), sender);
    // }

    transfer_balances(sender, to, token_id);
    sub_balance(sender, asset(10000, symbol_type(S(4, OREINST))));
    add_balance(to, asset(10000, symbol_type(S(4, OREINST))), sender);
    _tokens.modify(tokenitr, 0, [&](auto &a) {
        a.owner = to;
    });
}

void instrument::revoke(account_name revoker, uint64_t token_id)
{
    require_auth(revoker);

    //Checking if the token exists.
    auto tokenitr = _tokens.find(token_id);
    eosio_assert(tokenitr != _tokens.end(), "Token doesn't exists");

    eosio_assert(tokenitr->owner == revoker, "The revoker account doesn't have authority to revoke the instrument");

    eosio_assert(tokenitr->revoked == false, "Token is already revoked");

    _tokens.modify(tokenitr, 0, [&](auto &t) {
        t.revoked = true;
    });
}

// delets an instrument only if it's mutability is 2
// NOTE: this should result in changes in the following tables :
// tokens   - burnt token gets removed from the table
// account  - instrument owner's list of owned instruments get updated
// accounts - OREINST symbol balance gets updated
void instrument::burn(account_name burner, uint64_t token_id)
{
    require_auth(burner);
    bool from = false;

    // Checking if the token exists.
    auto tokenitr = _tokens.find(token_id);
    eosio_assert(tokenitr != _tokens.end(), "Token doesn't exists");

    eosio_assert(tokenitr->owner == burner, "The burner account doesn't have authority to delete the instrument");

    eosio_assert(tokenitr->instrument.mutability == 2, "Instrument is not mutable");

    transfer_balances(burner, 0, token_id);
    sub_balance(burner, asset(10000, symbol_type(S(4, OREINST))));

    _tokens.erase(tokenitr);
}

// -CUSTOM_CODE-it replicates the create function of ore.standard_token
// Creates a new currency OREINST
void instrument::create(account_name issuer,
                        asset maximum_supply)
{
    require_auth(_self);

    // Symbol is hardcoded here to prevent creating any other symbol than OREINST
    // auto sym = "maximum_supply.symbol";
    eosio::symbol_type sym = eosio::string_to_symbol(4, "OREINST");

    eosio_assert(maximum_supply.symbol == sym, "symbol name must be ORINST");
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "invalid supply");
    eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, sym.name());
    auto existing = statstable.find(sym.name());
    eosio_assert(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = sym;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
}

// -CUSTOM_CODE-it replicates the issue function of ore.standard_token except the inline transfer action present in ore.standard_token
// issue OREINST to an account
void instrument::issue(account_name to, asset quantity, string memo)
{
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    auto sym_name = sym.name();
    stats statstable(_self, sym_name);
    auto existing = statstable.find(sym_name);
    eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
    const auto &st = *existing;

    require_auth(st.issuer);
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must issue positive quantity");

    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify(st, 0, [&](auto &s) {
        s.supply += quantity;
    });

    add_balance(st.issuer, quantity, st.issuer);

    if (to != st.issuer)
    {
        sub_balance(st.issuer, quantity);
        add_balance(to, quantity, st.issuer);
    }
}

// -CUSTOM_CODE-it replicates the sub_balance function of ore.standard_token
void instrument::sub_balance(account_name owner, asset value)
{
    accounts from_acnts(_self, owner);

    const auto &from = from_acnts.get(value.symbol.name(), "no balance object found");
    eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

    if (from.balance.amount == value.amount)
    {
        from_acnts.erase(from);
    }
    else
    {
        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }
}

// -CUSTOM_CODE-it replicates the sub_balance_from function of ore.standard_token
// NOTE: Uncomment and use in future if required
// It is used by transfer_from account to specify the RAM payer as the "sender" account and not the "owner" account as in the sub_balance function
// NOTE: used by instrument::approve action
// void instrument::sub_balance_from(account_name sender, account_name owner, asset value)
// {
//     accounts from_acnts(_self, owner);

//     const auto &from = from_acnts.get(value.symbol.name(), "no balance object found");
//     eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

//     if (from.balance.amount == value.amount)
//     {
//         from_acnts.erase(from);
//     }
//     else
//     {
//         from_acnts.modify(from, sender, [&](auto &a) {
//             a.balance -= value;
//         });
//     }
// }

// -CUSTOM_CODE-it replicates the add_balance function of ore.standard_token
void instrument::add_balance(account_name owner, asset value, account_name ram_payer)
{
    accounts to_acnts(_self, owner);
    auto to = to_acnts.find(value.symbol.name());
    if (to == to_acnts.end())
    {
        to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
        });
    }
    else
    {
        to_acnts.modify(to, 0, [&](auto &a) {
            a.balance += value;
        });
    }
}

EOSIO_ABI(instrument, (transfer)(mint)(checkright)(createinst)(updateinst)(update)(revoke)(burn)(create)(issue))
