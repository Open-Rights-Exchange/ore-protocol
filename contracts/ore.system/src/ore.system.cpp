#include "ore.system.hpp"

// Migration
ACTION oresystem::migrate()
{
    require_auth(_self);
    while(_prices.begin() != _prices.end()) {
        _prices.erase(_prices.begin());
    }
    auto priceitr = _prices.begin();
    check(priceitr == _prices.end(), "pricetable has to be empty for migration");
    auto tieritr = _tiers.begin();
    check(tieritr == _tiers.end(), "tiertable has to be empty for migration");

    _prices.emplace(_self, [&](auto &p) {
        p.pricename = minimal_account_price;
        p.price = asset(100000, symbol(symbol_code("ORE"), 4));
    });

    _tiers.emplace(_self, [&](auto &t) {
        t.key = 1;
        t.ramfactor = 10000; 
        t.rambytes = 4070;
        t.netamount = asset(0, symbol(symbol_code("ORE"), 4));
        t.cpuamount = asset(0, symbol(symbol_code("ORE"), 4));
    });
}

// Setting or updating a price of an entity that can be bought on chain (i.e. accounts, instruments, tokens)
ACTION oresystem::setprice(name pricename, asset price)
{
    require_auth(_self);

    auto priceitr = _prices.find(pricename.value);

    if (priceitr == _prices.end())
    {
        _prices.emplace(_self, [&](auto &p) {
            p.pricename = pricename;
            p.price = price;
        });
    }
    else
    {
        _prices.modify(priceitr, _self, [&](auto &p) {
            p.price = price;
        });
    }
}

// Setting or updating a price of an entity that can be bought on chain (i.e. accounts, instruments, tokens)
ACTION oresystem::settier(uint64_t key, uint64_t ramfactor, uint64_t rambytes, asset netamount, asset cpuamount)
{
    require_auth(_self);

    auto tieritr = _tiers.find(key);

    if (tieritr == _tiers.end())
    {
        _tiers.emplace(_self, [&](auto &t) {
            t.key = key;
            t.ramfactor = ramfactor;
            t.rambytes = rambytes;
            t.netamount = netamount;
            t.cpuamount = cpuamount;
        });
    }
    else
    {
        _tiers.modify(tieritr, _self, [&](auto &t) {
            t.ramfactor = ramfactor;
            t.rambytes = rambytes;
            t.netamount = netamount;
            t.cpuamount = cpuamount;
        });
    }
}

ACTION oresystem::createoreacc(name creator,
                               name newname,
                               public_key &ownerkey,
                               public_key &activekey,
                               uint64_t tier,
                               name referral)
{
    require_auth(creator);
    authority ownerauth{.threshold = 1, .keys = {key_weight{ownerkey, 1}}, .accounts = {}, .waits = {}};
    authority activeauth{.threshold = 1, .keys = {key_weight{activekey, 1}}, .accounts = {}, .waits = {}};

    auto tieritr = _tiers.find(tier);
    check(tieritr != _tiers.end(), "No tier found");

    asset createprice = getPrice(minimal_account_price);
    // Calculate createprice from minimal account price and tier requirements
    createprice.amount = uint64_t((createprice.amount * tieritr->ramfactor) / 10000) + tieritr->netamount.amount + tieritr->cpuamount.amount;

    tierinfotable _tierinfo(_self, newname.value);
    _tierinfo.emplace(_self, [&](auto &ti) {
        ti.pricekey = tier;
        ti.createprice = createprice;
    });

    if (referral != name(""))
    {
        referralstatstable _stats(_self, referral.value);
        auto statsitr = _stats.find(tier);

        if (statsitr != _stats.end())
        {
            _stats.modify(statsitr, _self, [&](auto &s) {
                s.count += 1;
            });
        }
        else
        {
            _stats.emplace(_self, [&](auto &s) {
                s.pricekey = tier;
                s.count = 1;
            });
        }

        referrallogtable _log(_self, _self.value);
        _log.emplace(_self, [&](auto &l) {
            l.newaccount = newname;
            l.referral = referral;
        });
    }

    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n,
        "stake"_n,
        make_tuple(creator, newname, createprice, std::string("ore staked")))
        .send();
    //***
    accounts::newaccount new_account = accounts::newaccount{
        .creator = sys_payer,
        .name = newname,
        .owner = ownerauth,
        .active = activeauth};

    action(
        permission_level{sys_payer, "active"_n},
        "eosio"_n,
        "newaccount"_n,
        new_account)
        .send();

    action(
        permission_level{sys_payer, "active"_n},
        "eosio"_n,
        name("buyrambytes"),
        make_tuple(sys_payer, newname, tieritr->rambytes))
        .send();

    asset sys_stake_net, sys_stake_cpu;
    sys_stake_net.amount = tieritr->netamount.amount;
    sys_stake_cpu.amount = tieritr->cpuamount.amount;
    sys_stake_net.symbol = sys_symbol;
    sys_stake_cpu.symbol = sys_symbol;
    if(sys_stake_net.amount > 0 || sys_stake_cpu.amount > 0) {
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("delegatebw"),
            make_tuple(sys_payer, newname, sys_stake_net, sys_stake_cpu, false))
            .send();
    }
}


ACTION oresystem::chgacctier(name payer, name account, uint64_t tier)
{
    require_auth(payer);

    auto newTierItr = _tiers.find(tier);
    asset newprice = getPrice(name("minimumaccnt"));
    newprice.amount = uint64_t((newprice.amount * newTierItr->ramfactor) / 100) + newTierItr->netamount.amount + newTierItr->cpuamount.amount;

    asset currentCpu = getAccountCpu(account);
    asset currentNet = getAccountNet(account);
    uint64_t currentRambytes = getAccountRamBytes(account);

    asset cpuNetDelta;
    cpuNetDelta.symbol = core_symbol;
    uint64_t ramDelta;
    if (newTierItr->cpuamount.amount > currentCpu.amount)
    {
        cpuNetDelta.amount = newTierItr->cpuamount.amount - currentCpu.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("delegatebw"),
            make_tuple(sys_payer, account, asset(0, core_symbol), cpuNetDelta, false))
            .send();
    }
    else if (newTierItr->cpuamount.amount < currentCpu.amount)
    {
        cpuNetDelta.amount = currentCpu.amount - newTierItr->cpuamount.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("undelegatebw"),
            make_tuple(sys_payer, account, asset(0, core_symbol), cpuNetDelta))
            .send();
    }
    if (newTierItr->netamount.amount > currentNet.amount)
    {
        cpuNetDelta.amount = newTierItr->netamount.amount - currentNet.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("delegatebw"),
            make_tuple(sys_payer, account, cpuNetDelta, asset(0, core_symbol), false))
            .send();
    }
    else if (newTierItr->netamount.amount < currentNet.amount)
    {
        cpuNetDelta.amount = currentNet.amount - newTierItr->netamount.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("undelegatebw"),
            make_tuple(sys_payer, account, cpuNetDelta, asset(0, core_symbol)))
            .send();
    }
    if (newTierItr->rambytes > (currentRambytes + 14))
    {
        ramDelta = newTierItr->rambytes - currentRambytes;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("buyrambytes"),
            make_tuple(sys_payer, account, ramDelta))
            .send();
    }
    else if ((newTierItr->rambytes + 14) < currentRambytes)
    {
        ramDelta = currentRambytes - newTierItr->rambytes;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("sellram"),
            make_tuple(account, ramDelta))
            .send();
    }
    asset orePriceDelta;
    asset sysUsageDelta;
    tierinfotable _tierinfo(_self, account.value);
    auto oldTierInfoItr = _tierinfo.begin();
    name oldStaker = getStakerName(account);
    if (_tierinfo.begin() == _tierinfo.end())
    {
        action(
            permission_level{ore_system, "active"_n},
            "eosio.token"_n,
            "stake"_n,
            make_tuple(payer, account, newprice, std::string("ore staked")))
            .send();
    }
    else if (oldStaker == name(""))
    {
        check(newprice == oldTierInfoItr->createprice, "This account needs to be migrated first. Call changetier with same tier or same createprice.");
        action(
            permission_level{ore_system, "active"_n},
            "eosio.token"_n,
            "stake"_n,
            make_tuple(payer, account, newprice, std::string("ore staked")))
            .send();
        if(ore_lock != payer) {
            action(
            permission_level{ore_lock, "active"_n},
            "eosio.token"_n,
            "transfer"_n,
            make_tuple(ore_lock, payer, oldTierInfoItr->createprice, std::string("ore unlocked - (migration)")))
            .send();
        }
        _tierinfo.erase(oldTierInfoItr);
    }
    else
    {
        if (newprice > oldTierInfoItr->createprice)
        {
            if (oldStaker == payer)
            {
                orePriceDelta = newprice - oldTierInfoItr->createprice;
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "stake"_n,
                    make_tuple(payer, account, orePriceDelta, std::string("ore staked")))
                    .send();
            }
            else
            {
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "unstake"_n,
                    make_tuple(oldStaker, account, oldTierInfoItr->createprice, std::string("ore unstaked")))
                    .send();

                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "stake"_n,
                    make_tuple(payer, account, newprice, std::string("ore staked")))
                    .send();
            }
        }
        else if (newprice < oldTierInfoItr->createprice)
        {
            check(payer == account, "To downgrade tier, payer has to be owner");
            if (oldStaker == account)
            {
                orePriceDelta = oldTierInfoItr->createprice - newprice;
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "unstake"_n,
                    make_tuple(payer, account, orePriceDelta, std::string("ore unstaked")))
                    .send();
            }
            else
            {
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "unstake"_n,
                    make_tuple(oldStaker, account, oldTierInfoItr->createprice, std::string("ore unstaked")))
                    .send();

                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "stake"_n,
                    make_tuple(account, account, newprice, std::string("ore staked")))
                    .send();
            }
        }
        _tierinfo.erase(oldTierInfoItr);
    }

    _tierinfo.emplace(sys_payer, [&](auto &t) {
        t.pricekey = tier;
        t.createprice = newprice;
    });
}

ACTION oresystem::createtoken(const name& payer, const asset& maximum_supply) {

    asset price = getPrice(name("apptoken.ore"));

    action(
        permission_level{payer, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        make_tuple(payer, ore_system, price, std::string("create token payment")))
        .send();

    action(
        permission_level{ore_system, "active"_n},
        "apptoken.ore"_n,
        "create"_n,
        make_tuple(payer, maximum_supply))
        .send();

}

// namespace oresystem

EOSIO_DISPATCH(oresystem, (migrate)(setprice)(settier)(createoreacc)(chgacctier)(createtoken))