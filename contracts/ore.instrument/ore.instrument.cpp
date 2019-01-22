#include "ore.instrument.hpp"
using namespace eosio;

/*
    Creates new instrument
    NOTE: this should result in changes in the following tables :
    tokens   - add new token
    account  - instrument owner's list of owned instruments get updated
    accounts - OREINST symbol balance gets updated

    mint action internally calls a deferred transaction with 2 types of actions:
    checkright  - for each right in the instrument object, mint instrument calls check right within a deferred transaction
    createinst  - once all the rights are checked, the last action in the deferred transaction is createinst which adds the instrument to the tokens table
    NOTE: if any of the checkright action fails, it will cancel the deferred transaction and the creatinst action will not be called. Hence no instrument will be created.

    owner - owner of the new instrument
    minter - account authorized to mint the instrument's rights
    either the minter or the owner should own the right (or be whitelisted by the owner of the right)
 */
ACTION instrument::mint(name minter, name owner, instrument_data instrument,
                      uint64_t start_time, uint64_t end_time, uint64_t instrumentId = 0)
{
    // Checking if the minter has the required authorization
    require_auth(minter);

    string msg = "owner account does not exist " + owner.to_string();
    eosio_assert(is_account(owner), msg.c_str());

    // if an instrument_template name is passed-in, look from an instrument with the same name on the chain
    // ...if one exists, the new instrument will be a copy of that one
    // All instruments with the same template name will have the same data - only the dates may be different
    auto hashtable = _tokens.get_index<"templatehash"_n>();
    auto item = hashtable.find(hashStringToInt(instrument.instrument_template));

    if (instrument.rights.size() == 0)
    {
        eosio_assert(false, "Right array is empty");
    }

    // If instrumentId value passed as 0, next available primate key will be automatically assigned as instrumentId
    // So, instrumentId can't be set to 0
    if (instrumentId == 0)
    {
        if (_tokens.available_primary_key() == 0) //first instrument created
        {
            instrumentId = 1;
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

    // ------- Copy an existing intstrument from a template
    // If an instrument already exists with the given template name
    // ... copy the instrument data from the existing instrument
    // ... the owner of the new instrument is the same as existing instrument (you can't make a copy of someone else's template)
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
        uint64_t transaction_id = instrumentId;

        // Adding createinst action to the deferred transaction to add the new instrument to the tokens table
        create_instrument.actions.emplace_back(
            permission_level{"instr.ore"_n, "active"_n}, "instr.ore"_n, "createinst"_n,
            std::make_tuple(minter,
                            owner,
                            instrumentId,
                            instrument,
                            start_time,
                            end_time,
                            transaction_id));

        // send deferred transaction
        create_instrument.send(transaction_id, minter);
    }
    else
    {
        // ------- Create a new intstrument
        // create a deferred transaction object to check rights and add instrument to the tokens table
        transaction deferred_instrument{};

        // create an unique id for the deferred transaction
        // this deferred_transaction_id is different than the actual completed transaction Id
        uint64_t deferred_transaction_id = instrumentId;

        // The creation of this instrument is a series of steps grouped into one deferred transaction

        // Step 1 - add one action for each right to check if the issuer is approved to issue an instrument with that right
        for (int i = 0; i < instrument.rights.size(); i++)
        {
            deferred_instrument.actions.emplace_back(
                permission_level{"instr.ore"_n, "active"_n}, _self, "checkright"_n,
                std::make_tuple(
                    minter,
                    instrument.issuer,
                    instrument.rights[i].right_name,
                    deferred_transaction_id));
        }

        // Step 2 - if all the prior actions are sucessful (did not cancel because a right was invalid)
        // ... create the instrument as the last step in the transaction chain (using the createinst action)
        deferred_instrument.actions.emplace_back(
            permission_level{"instr.ore"_n, "active"_n}, _self, "createinst"_n,
            std::make_tuple(minter,
                            owner,
                            instrumentId,
                            instrument,
                            start_time,
                            end_time));

        // Step 3 - send deferred transaction
        deferred_instrument.send(deferred_transaction_id, minter);
    }
}

/*
    createinst creates a row in the instruments table or modifies an already existing row in the instrument table for the given instrument id
    This is called by the mint/update action - as the last step in the list of deferred transactions
    This can only be called within the instrument contract (requires _self for instr.ore) 
*/
ACTION instrument::createinst(name minter, name owner, uint64_t instrumentId, instrument_data instrument, uint64_t start_time, uint64_t end_time)
{
    require_auth(_self);
    auto accountitr = _account.find(owner.value);

    // We track every instrumentId a user owns in the accounts table - along with total count (balance) for each user
    // The first time a user creates an instrument, create a record in the accounts table in this contract
    if (accountitr == _account.end())
    {
        _account.emplace(_self, [&](auto &a) {
            a.owner = owner;
            a.balance = 0;
            print("new instrument account: ", a.primary_key(), "\n");
        });
        accountitr = _account.find(owner.value);
    }

    auto tokenitr = _tokens.find(instrumentId);

    // if there is no existing instrument with the input instrument id 
    if(tokenitr == _tokens.end()){
        // all instruments are stored in the tokens table
        _tokens.emplace(_self, [&](auto &a) {
            a.id = instrumentId;
            a.owner = owner;
            a.minted_by = minter;
            a.minted_at = now();;
            a.instrument = instrument;
            a.revoked = false;
            a.start_time = start_time;
            a.end_time = end_time;
            a.template_hash = hashStringToInt(instrument.instrument_template);
            a.class_hash = hashStringToInt(instrument.instrument_class);
        });

        // increasing the account balance (total token count)
        _account.modify(accountitr, same_payer, [&](auto &a) {
            a.balance++;
            a.instruments.push_back(instrumentId);
        });

        print("action:mint Created new instrument: type: " + instrument.instrument_class + " id: " + to_string(instrumentId) + " for: " + owner.to_string() + "\n");

        // transfer 1 OREINST from the issuer account for OREINST to the owner account of instrument
        sub_balance(_self, asset(10000, symbol(symbol_code("OREINST"),4)));
        add_balance(owner, asset(10000, symbol(symbol_code("OREINST"),4)), _self);
    } else {
       // update an already existing instrument
        _tokens.modify(tokenitr, same_payer, [&](auto &a) {
            a.id = instrumentId;
            a.owner = owner;
            a.minted_by = a.minted_by;
            a.minted_at = now();
            a.instrument = instrument;
            a.revoked = false;
            a.start_time = start_time;
            a.end_time = end_time;
            a.template_hash = hashStringToInt(instrument.instrument_template);
            a.class_hash = hashStringToInt(instrument.instrument_class);
        });

        print("action:update Updated instrument: type: " + instrument.instrument_class + " id: " + to_string(instrumentId) + " for: " + owner.to_string() + "\n");
    }
}

/*
    Checks that the issuer and owner are authorized to issue an instrument that includes this right (either the owner or in whitelist)
    This is called by the mint action - as part of a deferred transaction during the minting process
    This is called once for each right to be added to the instrument
    This can only be called within the instrument contract (requires _self for instr.ore) 
*/
ACTION instrument::checkright(name minter, name issuer, string rightname, uint64_t deferred_transaction_id = 0)
{
    require_auth(_self);

    string msg;

    rights_registry rights_contract("rights.ore"_n,"rights.ore"_n,_ds);

    // check that right exists in the rights registry
    auto rightitr = rights_contract.find_right_by_name(rightname);

    if (rightitr.owner.value == 0)
    {
        if (deferred_transaction_id != 0)
        {
           cancel_deferred(deferred_transaction_id);
        }
        msg = "right:" + rightname + " doesn't exist";
        eosio_assert(rightitr.owner.value != 0, msg.c_str());
    }

    // check if the minter of the instrument is the issuer of the right
    bool minter_owns_right = rightitr.owner == minter;
    if (!minter_owns_right)
    {
        auto position_in_whitelist = std::find(rightitr.issuer_whitelist.begin(), rightitr.issuer_whitelist.end(), minter); 
        //if minter is not in whitelist, cancel the entire mint transaction
        if (position_in_whitelist == rightitr.issuer_whitelist.end())
        {
            if (deferred_transaction_id != 0)
            {
                cancel_deferred(deferred_transaction_id);
            }
            msg = "Attempt to create instrument with right: " + rightname + " by minter: " + minter.to_string() + " who isn't whitelisted or owner of right";
            eosio_assert(false, msg.c_str());
        }
    }

    // check if the issuer of the instrument is the owner of the right
    bool issuer_owns_right = rightitr.owner == issuer;
    if (!issuer_owns_right)
    {
        auto issuer_in_whitelist = std::find(rightitr.issuer_whitelist.begin(), rightitr.issuer_whitelist.end(), issuer);
        //if issuer is not in whitelist, cancel the entire mint transaction
        if (issuer_in_whitelist == rightitr.issuer_whitelist.end())
        {
            if (deferred_transaction_id != 0)
            {
                cancel_deferred(deferred_transaction_id);
            }
            msg = "Attempt to create instrument with right: " + rightname + " by issuer: " + issuer.to_string() + " who isn't whitelisted or owner of right";
            eosio_assert(false, msg.c_str());
        }
    }
}

/* 
    updates an instrument (in the tokens table)
    the instrument token gets updated depending on the mutabiility
    mutability = 0 - completely immutable
    mutability = 1 - start_time and/or end_time can be updated
    mutability = 2 - everything mutable except the owner can't be updated (calls createinst action as a deferred transaction in this case to update instrument )
*/
ACTION instrument::update(name updater, string instrument_template, instrument_data instrument = {},
                        uint64_t instrument_id = 0, uint64_t start_time = 0, uint64_t end_time = 0)
{ 
    require_auth(updater);
    uint64_t new_start;
    uint64_t new_end;

    instrument::token item;

    //find existing instrument by id or template
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

    rights_registry rights_contract("rights.ore"_n,"rights.ore"_n,_ds);

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

    // mutability = 1 - update dates
    if (item.instrument.mutability == 1)
    {
        // update the instrument token in the tokens table 
        _tokens.modify(tokenitr, same_payer, [&](auto &a) {
            a.start_time = new_start;
            a.end_time = new_end;
        });

        print("action:update Updated instrument: type: " + item.instrument.instrument_class + " id: " + to_string(item.id) + " for: " + item.owner.to_string() + "\n");
    }

    // mutability = 2 - update anything
    if (item.instrument.mutability == 2)
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

        uint64_t deferred_trx_id = item.id;

        for (int i = 0; i < item.instrument.rights.size(); i++)
        {
            deferred_instrument.actions.emplace_back(
                permission_level{"instr.ore"_n, "active"_n}, _self, "checkright"_n,
                std::make_tuple(
                    updater,
                    instrument.issuer,
                    instrument.rights[i].right_name,
                    deferred_trx_id));
        }

        // Adding createinst action to the deferred transaction to add the new instrument to the tokens table
        deferred_instrument.actions.emplace_back(
            permission_level{"instr.ore"_n, "active"_n}, _self, "createinst"_n,
            std::make_tuple(updater,
                            item.owner,
                            instrument_id,
                            instrument,
                            start_time,
                            end_time));

        // send deferred transaction
        deferred_instrument.send(deferred_trx_id, _self);
    }
}

/* 
    transfers an instrument (owner field for the instrument gets updated to the new owner in the tokens table)
 */
ACTION instrument::transfer(name sender, name to, uint64_t instrument_id)
{

    require_auth(sender);

    string msg;

    //find token
    auto tokenitr = _tokens.find(instrument_id);

    msg = "Instrument Id" + to_string(instrument_id) + "doesn't exist";
    eosio_assert(tokenitr != _tokens.end(), msg.c_str());

    msg = "Sender account is not allowed to transfer the instrument " + sender.to_string();
    eosio_assert(tokenitr->owner == sender, msg.c_str());

    msg = "Instrument Id " + to_string(instrument_id) + " has been previously revoked";
    eosio_assert(tokenitr->revoked == false, msg.c_str());

    // transfer balance in the accounts table
    transfer_balances(sender, to, instrument_id);

    // transfer OREINST balance
    sub_balance(sender, asset(10000, symbol(symbol_code("OREINST"),4)));
    add_balance(to, asset(10000, symbol(symbol_code("OREINST"),4)), sender);
    _tokens.modify(tokenitr, same_payer, [&](auto &a) {
        a.owner = to;
    });
}

// revokes an instrument - A revoked instrument is no longer active and cannot be used
ACTION instrument::revoke(name revoker, uint64_t instrument_id)
{
    require_auth(revoker);

    string msg;

    //Checking if the token exists.
    auto tokenitr = _tokens.find(instrument_id);

    msg = "Instrument Id" + to_string(instrument_id) + "doesn't exist";
    eosio_assert(tokenitr != _tokens.end(), msg.c_str());

    msg = "The account " + revoker.to_string() + "doesn't have authority to revoke the instrument";
    eosio_assert(tokenitr->owner == revoker, msg.c_str());

    msg = "Instrument Id" + to_string(instrument_id) + "has been previously revoked";
    eosio_assert(tokenitr->revoked == false, msg.c_str());

    _tokens.modify(tokenitr, same_payer, [&](auto &t) {
        t.revoked = true;
    });
}

/* 
   deletes an instrument (from the tokens table)
   deletes only if it's mutability is 2 ( as mutability 2 means we can change anything)
*/
ACTION instrument::burn(name burner, uint64_t instrument_id)
{
    require_auth(burner);

    string msg;
    bool from = false;

    // Checking if the token exists.
    auto tokenitr = _tokens.find(instrument_id);

    msg = "Instrument Id" + to_string(instrument_id) + "doesn't exist";
    eosio_assert(tokenitr != _tokens.end(), msg.c_str());

    msg = "The account " + burner.to_string() + "doesn't have authority to burn the instrument";
    eosio_assert(tokenitr->owner == burner, msg.c_str());

    msg = "Instrument Id" + to_string(instrument_id) + "is not mutable and cannot be burned.";
    eosio_assert(tokenitr->instrument.mutability == 2, msg.c_str());

    transfer_balances(burner, same_payer, instrument_id);
    sub_balance(burner, asset(10000, symbol(symbol_code("OREINST"),4)));

    _tokens.erase(tokenitr);
}

/*
    -CUSTOM_CODE- it replicates the create function of ore.standard_token
    creates a new currency OREINST
*/
ACTION instrument::create(name issuer,
                        asset maximum_supply)
{
    require_auth(_self);

    // Symbol is hardcoded here to prevent creating any other symbol than OREINST
    // auto sym = "maximum_supply.symbol";
    eosio::symbol sym = symbol(symbol_code("OREINST"),4);

    eosio_assert(maximum_supply.symbol == sym, "symbol name must be ORINST");
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "invalid supply");
    eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    eosio_assert(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = sym;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
}

/* 
    -CUSTOM_CODE-it replicates the issue function of ore.standard_token except the inline transfer action present in ore.standard_token
    issue OREINST to an account 
*/
ACTION instrument::issue(name to, asset quantity, string memo)
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
        sub_balance(st.issuer, quantity);
        add_balance(to, quantity, st.issuer);
    }
}

/* 
    -CUSTOM_CODE-it replicates the sub_balance function of ore.standard_token
    removes OREINST from an account
 */
void instrument::sub_balance(name owner, asset value)
{
    accounts from_acnts(_self, owner.value);

    const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
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

/* 
    -CUSTOM_CODE-it replicates the add_balance function of ore.standard_token
    adds OREINST to an account
*/
void instrument::add_balance(name owner, asset value, name ram_payer)
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

EOSIO_DISPATCH(instrument, (transfer)(mint)(checkright)(createinst)(update)(revoke)(burn)(create)(issue))
