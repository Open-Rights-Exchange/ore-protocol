#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/ignore.hpp>
#include "eosio/transaction.hpp"
#include "accounts.h"
#include "common.h"
using namespace eosio;
using namespace std;
using namespace accounts;
using namespace common;
class [[eosio::contract("ore.system")]] oresystem : public contract
{
private:
  TABLE prices
   {
      name pricename;
      asset price;
      uint64_t primary_key() const { return pricename.value; }
      EOSLIB_SERIALIZE(prices, (pricename)(price))
   };
   typedef eosio::multi_index<"pricetable"_n, prices> pricetable;

   TABLE tiers
   {
      uint64_t key;
      uint64_t ramfactor;            // an integer value that represents a float number with 4 decimals ( eg. 10000 = 1.0000)
      uint64_t rambytes;            // initial amount of ram
      asset netamount;              // initial amount of net
      asset cpuamount;              // initial amount of cpu

      uint64_t primary_key() const { return key; }
      EOSLIB_SERIALIZE(tiers, (key)(ramfactor)(rambytes)(netamount)(cpuamount))
   };
   typedef eosio::multi_index<"tiertable"_n, tiers> tiertable;

   TABLE tierinfo
   {
      uint64_t pricekey;
      asset createprice;

      uint64_t primary_key() const { return pricekey; }
      EOSLIB_SERIALIZE(tierinfo, (pricekey)(createprice))
   };

   // May be erased when incentive is paid to referral account.
   TABLE reflog
   {
      name referral;
      name newaccount;

      uint64_t primary_key() const { return newaccount.value; }
      EOSLIB_SERIALIZE(reflog, (referral)(newaccount))
   };
   

   TABLE refstats
   {
      uint64_t pricekey;
      uint64_t count;

      uint64_t primary_key() const { return pricekey; }
      EOSLIB_SERIALIZE(refstats, (pricekey)(count))
   };

   /** 
    * Check Network Utilization < Emergency Threshold (%80)
    * if threshold is exceeded increase the price exponentially
    * IMPORTANT: emergency phase prices are temporary
    *    After upgraderam() is triggered and utilization normalizes back to < %50
    *    stakers will be able to get refund for the extra amount they paid during emergency phase
   */
   asset getPrice(name pricename) {
      auto priceitr = _prices.find(pricename.value);
      check(priceitr != _prices.end(), "No price found");
      asset price;
      float utilization = getRamUtilization();
      if(utilization < emergency_threshold ) {
         price = priceitr->price;
      } else {
         price = priceitr->price;
         price.amount = (uint64_t)(price.amount / ( 1 - ((utilization - emergency_threshold ) / (1.0F - emergency_threshold))));
      }
      return price;
   }

public:
   using contract::contract;
   oresystem(name receiver, name code, datastream<const char *> ds)
       : contract(receiver, code, ds), _prices(receiver, receiver.value), _tiers(receiver, receiver.value) {}

   ACTION migrate();
   ACTION setprice(name pricename, asset price);
   ACTION settier(uint64_t key, uint64_t ramfactor, uint64_t rambytes, asset netamount, asset cpuamount);
   ACTION createoreacc(name creator,
                        name newname,
                        public_key &ownerkey,
                        public_key &activekey,
                        uint64_t tier,
                        name referral);
   ACTION chgacctier(name payer, name account, uint64_t tier);

   ACTION createtoken(const name& payer, const asset& maximum_supply);

   pricetable _prices;
   tiertable _tiers;

   typedef eosio::multi_index<"tierinfo"_n, tierinfo> tierinfotable;
   typedef eosio::multi_index<"reflog"_n, reflog> referrallogtable;
   typedef eosio::multi_index<"refstats"_n, refstats> referralstatstable;

   //possibly different accounts that has different uses
   static constexpr name minimal_account_price{"minimalaccnt"_n};
   static constexpr name token_account{"eosio.token"_n};
   static constexpr name ore_system{"system.ore"_n};
   static constexpr name ore_lock{"lock.ore"_n};
   static constexpr name sys_lock{"lock.ore"_n};
   static constexpr name sys_payer{"system.ore"_n};
   static constexpr symbol ore_symbol = symbol(symbol_code("ORE"), 4);
   static constexpr symbol sys_symbol = symbol(symbol_code("SYS"), 4);

   static constexpr float emergency_threshold = 0.8;
};