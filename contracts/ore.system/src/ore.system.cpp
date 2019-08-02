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
                               name referral
                               )
{

    authority ownerauth{.threshold = 1, .keys = {key_weight{ownerkey, 1}}, .accounts = {}, .waits = {}};
    authority activeauth{.threshold = 1, .keys = {key_weight{activekey, 1}}, .accounts = {}, .waits = {}};

    auto priceitr = _prices.find(pricekey);
    check(priceitr != _prices.end(), "No price table");

    tierinfotable _tiers(_self, newname.value);
    _tiers.emplace(_self, [&](auto &t) {
        t.pricekey = pricekey;
        t.createprice = priceitr->createprice;
    });
    if(referral != name("")) {
        referralstatstable _stats(_self, referral.value);
        auto statsitr = _stats.find(pricekey);
        
        if(statsitr != _stats.end()) {
            _stats.modify(statsitr, _self, [&](auto &s) {
                s.count += 1;
            });
        } else {
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

    asset ramprice = common::getRamCost(priceitr->rambytes);
    asset sys_stake_net, sys_stake_cpu;
    sys_stake_net.amount = priceitr->netamount.amount;
    sys_stake_cpu.amount = priceitr->cpuamount.amount;
    sys_stake_net.symbol = sys_symbol;
    sys_stake_cpu.symbol = sys_symbol;

    asset syslock;

    syslock.symbol = sys_symbol;
    syslock.amount = priceitr->createprice.amount - (sys_stake_net.amount + sys_stake_cpu.amount + ramprice.amount);

    action(
        permission_level{creator, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        make_tuple(creator, ore_lock, priceitr->createprice, std::string("ore lock")))
        .send();

    action(
        permission_level{ore_system, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        make_tuple(sys_payer, sys_lock, syslock, std::string("ore savings")))
        .send();

    accounts::newaccount new_account = accounts::newaccount{
        .creator = creator,
        .name = newname,
        .owner = ownerauth,
        .active = activeauth};
    print("before newacc");

    action(
        permission_level{creator, "active"_n},
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

ACTION oresystem::changetier(name payer, name account, uint64_t pricekey)
{
    require_auth(sys_payer);

    auto newPriceItr = _prices.find(pricekey);

    asset currentCpu = getAccountCpu(account);
    asset currentNet = getAccountNet(account);
    uint64_t currentRambytes = getAccountRamBytes(account);

    asset cpuDelta, netDelta;
    uint64_t ramDelta;

    if(newPriceItr->cpuamount.amount > currentCpu.amount) {
        cpuDelta.amount = newPriceItr->cpuamount.amount - currentCpu.amount;
        cpuDelta.symbol = core_symbol;
        action(
        permission_level{sys_payer, "active"_n},
        "eosio"_n,
        name("delegatebw"),
        make_tuple(sys_payer, account, asset(0, core_symbol), cpuDelta, false))
        .send();
    } else if (newPriceItr->cpuamount.amount < currentCpu.amount) {
        cpuDelta.amount = currentCpu.amount - newPriceItr->cpuamount.amount;
        cpuDelta.symbol = core_symbol;
        action(
        permission_level{sys_payer, "active"_n},
        "eosio"_n,
        name("undelegatebw"),
        make_tuple(sys_payer, account, asset(0, core_symbol), cpuDelta))
        .send();
    }
    print("before netamount");
    if(newPriceItr->netamount.amount > currentNet.amount) {
        netDelta.amount = newPriceItr->netamount.amount - currentNet.amount;
        netDelta.symbol = core_symbol;
        action(
        permission_level{sys_payer, "active"_n},
        "eosio"_n,
        name("delegatebw"),
        make_tuple(sys_payer, account, netDelta, asset(0, core_symbol), false))
        .send();
    } else if (newPriceItr->netamount.amount < currentCpu.amount) {
        netDelta.amount = currentNet.amount - newPriceItr->netamount.amount;
        netDelta.symbol = core_symbol;
        action(
        permission_level{sys_payer, "active"_n},
        "eosio"_n,
        name("undelegatebw"),
        make_tuple(sys_payer, account, netDelta, asset(0, core_symbol)))
        .send();
    }
    print("before ramamount");
    if(newPriceItr->rambytes > currentRambytes) {
        ramDelta = newPriceItr->rambytes - currentRambytes;
        action(
            permission_level{sys_payer, "active"_n},
            "eosio"_n,
            name("buyrambytes"),
            make_tuple(sys_payer, account, ramDelta))
            .send();
    } else if (newPriceItr->rambytes < currentRambytes) {
        ramDelta = currentRambytes - newPriceItr->rambytes;
        action(
            permission_level{account, "active"_n},
            "eosio"_n,
            name("sellram"),
            make_tuple(account, ramDelta))
            .send();
    }

    asset orePriceDelta;
    tierinfotable _tiers(_self, account.value);
    auto oldTierItr = _tiers.begin();
    if(_tiers.begin() == _tiers.end()) {
        orePriceDelta = newPriceItr->createprice;
    } else {
        if(newPriceItr->createprice > oldTierItr->createprice) {
            orePriceDelta = newPriceItr->createprice - oldTierItr->createprice;
            action(
                permission_level{account, "active"_n},
                "eosio.token"_n,
                "transfer"_n,
                make_tuple(payer, ore_lock, orePriceDelta, std::string("ore lock")))
                .send();
            action(
                permission_level{sys_payer, "active"_n},
                "eosio.token"_n,
                "transfer"_n,
                make_tuple(sys_payer, sys_lock, asset(orePriceDelta.amount, core_symbol), std::string("ore savings")))
                .send();
        } else if (newPriceItr->createprice < oldTierItr->createprice) {
            orePriceDelta = oldTierItr->createprice - newPriceItr->createprice;
            action(
                permission_level{ore_lock, "active"_n},
                "eosio.token"_n,
                "transfer"_n,
                make_tuple(ore_lock, account, orePriceDelta, std::string("ore released from lock")))
                .send();
            action(
                permission_level{sys_lock, "active"_n},
                "eosio.token"_n,
                "transfer"_n,
                make_tuple(sys_lock, sys_payer, asset(orePriceDelta.amount, common::core_symbol), std::string("ore savings")))
                .send();
        }
        _tiers.erase(oldTierItr);
    }
    
    _tiers.emplace(sys_payer, [&](auto &t) {
            t.pricekey = pricekey;
            t.createprice = newPriceItr->createprice;
    });

    transaction deferred;
    
    deferred.actions.emplace_back(
      permission_level{get_self(),"active"_n},
      get_self(), "depletesys"_n, 
      std::make_tuple(account)
    );
    deferred.send(account.value, get_self());
}

ACTION oresystem::depletesys(name account)
{
    asset depleteSYSAmount = getSYSBalance(account);
    if(depleteSYSAmount.amount > 0) {
        action(
        permission_level{account, "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        make_tuple(account, sys_payer, depleteSYSAmount, std::string("remaining SYS will be taken from account"))
        ).send();
    }
}
// namespace oresystem

EOSIO_DISPATCH(oresystem, (setprice)(createoreacc)(changetier)(depletesys))