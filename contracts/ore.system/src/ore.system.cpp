#include "ore.system.hpp"

// Setting newaccount parameters and the price in terms of ORE
// IMPORTANT: since bwpricerate is uint, ORE price needs to be equal or more than required SYS
ACTION oresystem::setprice(asset createprice, uint64_t rambytes, asset netamount, asset cpuamount, uint64_t pricekey)
{
    require_auth(_self);

    auto priceitr = _prices.find(pricekey);

    if (priceitr == _prices.end())
    {
        _prices.emplace(_self, [&](auto &p) {
            p.key = pricekey;
            p.createprice = createprice;
            p.rambytes = rambytes;
            p.netamount = netamount;
            p.cpuamount = cpuamount;
        });
    }
    else
    {
        _prices.modify(priceitr, _self, [&](auto &p) {
            p.createprice = createprice;
            p.rambytes = rambytes;
            p.netamount = netamount;
            p.cpuamount = cpuamount;
        });
    }
}

ACTION oresystem::createoreacc(name creator,
                               name newname,
                               public_key &ownerkey,
                               public_key &activekey,
                               uint64_t pricekey,
                               name referral)
{
    require_auth(creator);
    authority ownerauth{.threshold = 1, .keys = {key_weight{ownerkey, 1}}, .accounts = {}, .waits = {}};
    authority activeauth{.threshold = 1, .keys = {key_weight{activekey, 1}}, .accounts = {}, .waits = {}};

    auto priceitr = _prices.find(pricekey);
    check(priceitr != _prices.end(), "No price table");

    tierinfotable _tiers(_self, newname.value);
    _tiers.emplace(_self, [&](auto &t) {
        t.pricekey = pricekey;
        t.createprice = priceitr->createprice;
    });
    if (referral != name(""))
    {
        referralstatstable _stats(_self, referral.value);
        auto statsitr = _stats.find(pricekey);

        if (statsitr != _stats.end())
        {
            _stats.modify(statsitr, _self, [&](auto &s) {
                s.count += 1;
            });
        }
        else
        {
            _stats.emplace(_self, [&](auto &s) {
                s.pricekey = pricekey;
                s.count = 1;
            });
        }

        referrallogtable _log(_self, _self.value);
        _log.emplace(_self, [&](auto &l) {
            l.newaccount = newname;
            l.referral = referral;
        });
    }

    //Get the ramprice and calculate the amount of SYS to be locked

    asset ramprice = getRamCost(priceitr->rambytes);
    asset sys_stake_net, sys_stake_cpu;
    sys_stake_net.amount = priceitr->netamount.amount;
    sys_stake_cpu.amount = priceitr->cpuamount.amount;
    sys_stake_net.symbol = sys_symbol;
    sys_stake_cpu.symbol = sys_symbol;

    uint64_t resourceCost = sys_stake_net.amount + sys_stake_cpu.amount + ramprice.amount;
    asset syslock;
    syslock.symbol = sys_symbol;

    if (priceitr->createprice.amount > resourceCost)
    {
        syslock.amount = priceitr->createprice.amount - resourceCost;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio.token"_n,
            "transfer"_n,
            make_tuple(sys_payer, sys_lock, syslock, std::string("sys locked")))
            .send();
        // If createprice is less than actual resource cost, sys_lock account HAS TO have enough balance to release price difference.
    }
    else if (priceitr->createprice.amount < resourceCost)
    {
        syslock.amount = resourceCost - priceitr->createprice.amount;
        action(
            permission_level{sys_lock, "active"_n},
            "eosio.token"_n,
            "transfer"_n,
            make_tuple(sys_lock, sys_payer, syslock, std::string("sys released")))
            .send();
    }

    //*** Changed GBT

    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n,
        "stake"_n,
        make_tuple(creator, newname, priceitr->createprice, std::string("ore lock")))
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
        make_tuple(sys_payer, newname, priceitr->rambytes))
        .send();

    action(
        permission_level{sys_payer, "active"_n},
        "eosio"_n,
        name("delegatebw"),
        make_tuple(sys_payer, newname, sys_stake_net, sys_stake_cpu, false))
        .send();
}

ACTION oresystem::chgacctier(name payer, name account, uint64_t pricekey)
{
    require_auth(payer);
    auto newPriceItr = _prices.find(pricekey);

    asset currentCpu = getAccountCpu(account);
    asset currentNet = getAccountNet(account);
    uint64_t currentRambytes = getAccountRamBytes(account);

    asset cpuNetDelta;
    cpuNetDelta.symbol = core_symbol;
    uint64_t ramDelta;
    if (newPriceItr->cpuamount.amount > currentCpu.amount)
    {
        cpuNetDelta.amount = newPriceItr->cpuamount.amount - currentCpu.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("delegatebw"),
            make_tuple(sys_payer, account, asset(0, core_symbol), cpuNetDelta, false))
            .send();
    }
    else if (newPriceItr->cpuamount.amount < currentCpu.amount)
    {
        cpuNetDelta.amount = currentCpu.amount - newPriceItr->cpuamount.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("undelegatebw"),
            make_tuple(sys_payer, account, asset(0, core_symbol), cpuNetDelta))
            .send();
    }
    if (newPriceItr->netamount.amount > currentNet.amount)
    {
        cpuNetDelta.amount = newPriceItr->netamount.amount - currentNet.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("delegatebw"),
            make_tuple(sys_payer, account, cpuNetDelta, asset(0, core_symbol), false))
            .send();
    }
    else if (newPriceItr->netamount.amount < currentNet.amount)
    {
        cpuNetDelta.amount = currentNet.amount - newPriceItr->netamount.amount;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("undelegatebw"),
            make_tuple(sys_payer, account, cpuNetDelta, asset(0, core_symbol)))
            .send();
    }
    if (newPriceItr->rambytes > (currentRambytes + 14))
    {
        ramDelta = newPriceItr->rambytes - currentRambytes;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("buyrambytes"),
            make_tuple(sys_payer, account, ramDelta))
            .send();
    }
    else if ((newPriceItr->rambytes + 14) < currentRambytes)
    {
        ramDelta = currentRambytes - newPriceItr->rambytes;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("sellram"),
            make_tuple(account, ramDelta))
            .send();
    }
    asset orePriceDelta;
    asset sysUsageDelta;
    tierinfotable _tiers(_self, account.value);
    auto oldTierItr = _tiers.begin();
    name oldStaker = getStakerName(account);
    if (_tiers.begin() == _tiers.end())
    {
        action(
            permission_level{ore_system, "active"_n},
            "eosio.token"_n,
            "stake"_n,
            make_tuple(payer, account, newPriceItr->createprice, std::string("ore lock")))
            .send();
    }
    else if (oldStaker == name(""))
    {
        check(newPriceItr->createprice == oldTierItr->createprice, "This account needs to be migrated first. Call changetier with same tier or same createprice.");
        action(
            permission_level{ore_system, "active"_n},
            "eosio.token"_n,
            "stake"_n,
            make_tuple(payer, account, newPriceItr->createprice, std::string("ore lock")))
            .send();
        if(ore_lock != payer) {
            action(
            permission_level{ore_lock, "active"_n},
            "eosio.token"_n,
            "transfer"_n,
            make_tuple(ore_lock, payer, newPriceItr->createprice, std::string("ore unlocked - (migration)")))
            .send();
        }
        _tiers.erase(oldTierItr);
    }
    else
    {
        if (newPriceItr->createprice > oldTierItr->createprice)
        {
            if (oldStaker == payer)
            {
                orePriceDelta = newPriceItr->createprice - oldTierItr->createprice;
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "stake"_n,
                    make_tuple(payer, account, orePriceDelta, std::string("ore lock")))
                    .send();
            }
            else
            {
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "unstake"_n,
                    make_tuple(oldStaker, account, oldTierItr->createprice, std::string("ore released from lock")))
                    .send();

                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "stake"_n,
                    make_tuple(payer, account, newPriceItr->createprice, std::string("ore lock")))
                    .send();
            }
        }
        else if (newPriceItr->createprice < oldTierItr->createprice)
        {
            check(payer == account, "To downgrade tier, payer has to be owner");
            if (oldStaker == account)
            {
                orePriceDelta = oldTierItr->createprice - newPriceItr->createprice;
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "unstake"_n,
                    make_tuple(payer, account, orePriceDelta, std::string("ore released from lock")))
                    .send();
            }
            else
            {
                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "unstake"_n,
                    make_tuple(oldStaker, account, oldTierItr->createprice, std::string("ore released from lock")))
                    .send();

                action(
                    permission_level{ore_system, "active"_n},
                    "eosio.token"_n,
                    "stake"_n,
                    make_tuple(account, account, newPriceItr->createprice, std::string("ore lock")))
                    .send();
            }
        }
        _tiers.erase(oldTierItr);
    }
    asset currentRamCost = getRamCost(currentRambytes);
    asset newRamCost = getRamCost(newPriceItr->rambytes);
    asset currentTotalCost;
    asset oldSysLock;
    asset newTotalCost;
    asset newSysLock;
    currentTotalCost.amount = currentRamCost.amount + currentNet.amount + currentCpu.amount;
    oldSysLock.amount = oldTierItr->createprice.amount - currentTotalCost.amount;
    newTotalCost.amount = newRamCost.amount + newPriceItr->netamount.amount + newPriceItr->cpuamount.amount;
    newSysLock.amount = newPriceItr->createprice.amount - newTotalCost.amount;

    if (newSysLock.amount > oldSysLock.amount)
    {
        action(
            permission_level{sys_payer, "active"_n},
            "eosio.token"_n,
            "transfer"_n,
            make_tuple(sys_payer, sys_lock, asset((newSysLock.amount - oldSysLock.amount), core_symbol), std::string("sys locked")))
            .send();
    }
    else if (oldSysLock.amount > newSysLock.amount)
    {
        action(
            permission_level{sys_lock, "active"_n},
            "eosio.token"_n,
            "transfer"_n,
            make_tuple(sys_lock, sys_payer, asset((oldSysLock.amount - newSysLock.amount), core_symbol), std::string("sys released")))
            .send();
    }

    _tiers.emplace(sys_payer, [&](auto &t) {
        t.pricekey = pricekey;
        t.createprice = newPriceItr->createprice;
    });
}

ACTION oresystem::createtoken(const name& payer, const asset& maximum_supply) {

    action(
        permission_level{payer, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        make_tuple(payer, ore_system, asset(10000, ore_symbol), std::string("create token payment")))
        .send();

    action(
        permission_level{ore_system, "active"_n},
        "apptoken.ore"_n,
        "create"_n,
        make_tuple(payer, maximum_supply))
        .send();

}

// namespace oresystem

EOSIO_DISPATCH(oresystem, (setprice)(createoreacc)(chgacctier)(createtoken))