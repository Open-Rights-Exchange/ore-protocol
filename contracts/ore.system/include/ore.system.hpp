#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/ignore.hpp>
#include "eosiolib/transaction.hpp"
#include "accounts.h"
#include "common.h"
using namespace eosio;
using namespace std;
using namespace accounts;
using namespace common;
class [[eosio::contract("ore.system")]] oresystem : public contract
{
private:
   TABLE oreprice
   {
      uint64_t key;
      asset createprice;            // newaccount price as ORE
      uint64_t rambytes;            // initial amount of ram
      asset netamount;              // initial amount of net
      asset cpuamount;              // initial amount of cpu

      uint64_t primary_key() const { return key; }
      EOSLIB_SERIALIZE(oreprice, (key)(createprice)(rambytes)(netamount)(cpuamount))
   };
   typedef eosio::multi_index<"pricetable"_n, oreprice> orepricetable;

   TABLE tierinfo
   {
      uint64_t pricekey;
      asset createprice;

      uint64_t primary_key() const { return pricekey; }
      EOSLIB_SERIALIZE(tierinfo, (pricekey)(createprice))
   };

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

public:
   using contract::contract;
   oresystem(name receiver, name code, datastream<const char *> ds)
       : contract(receiver, code, ds), _prices(receiver, receiver.value) {}

   ACTION setprice(asset createprice, uint64_t rambytes, asset netamount, asset cpuamount, uint64_t pricekey);
   ACTION createoreacc(name creator,
                        name newname,
                        public_key &ownerkey,
                        public_key &activekey,
                        uint64_t pricekey,
                        name referral );
   ACTION changetier(name payer, name account, uint64_t pricekey);
   ACTION depletesys(name account);

   orepricetable _prices;

   typedef eosio::multi_index<"tierinfo"_n, tierinfo> tierinfotable;
   typedef eosio::multi_index<"reflog"_n, reflog> referrallogtable;
   typedef eosio::multi_index<"refstats"_n, refstats> referralstatstable;

   //possibly different accounts that has different uses
   static constexpr name token_account{"eosio.token"_n};
   static constexpr name ore_system{"system.ore"_n};
   static constexpr name ore_lock{"lock.ore"_n};
   static constexpr name sys_lock{"lock.ore"_n};
   static constexpr name sys_payer{"system.ore"_n};
   static constexpr symbol ore_symbol = symbol(symbol_code("ORE"), 4);
   static constexpr symbol sys_symbol = symbol(symbol_code("SYS"), 4);
};

// 1 insturment system cost : 1 SYS      -----> 4 SYS
// ORE price : 5 ORE                     -----> 5 ORE