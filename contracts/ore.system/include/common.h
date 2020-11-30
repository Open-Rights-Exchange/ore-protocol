#pragma once

#include <eosio/eosio.hpp>
//#include <eosiolib/asset.hpp>
#include <eosio/singleton.hpp>

using namespace eosio;
using std::string;
using std::vector;

namespace common
{
   static const symbol S_RAM = symbol("RAMCORE", 4);
   static const symbol core_symbol = symbol("SYS", 4);
   static const symbol ore_symbol = symbol("ORE", 4);
   static const name ore_system = name("oresystem");

   /**********************************************/
   /***                                        ***/
   /***            RAM calculations            ***/
   /***                                        ***/
   /**********************************************/

   struct rammarket
   {
      asset supply;

      struct connector
      {
         asset balance;
         double weight = .5;
      };

      connector base;
      connector quote;

      uint64_t primary_key() const { return supply.symbol.raw(); }
   };

   typedef eosio::multi_index<"rammarket"_n, rammarket> RamInfo;

   struct blockchain_parameters {
      uint64_t	max_block_net_usage;
      uint32_t	target_block_net_usage_pct;
      uint32_t	max_transaction_net_usage;
      uint32_t	base_per_transaction_net_usage;
      uint32_t	net_usage_leeway;
      uint32_t	context_free_discount_net_usage_num;
      uint32_t	context_free_discount_net_usage_den;
      uint32_t	max_block_cpu_usage;
      uint32_t	target_block_cpu_usage_pct;
      uint32_t	max_transaction_cpu_usage;
      uint32_t	min_transaction_cpu_usage;
      uint32_t	max_transaction_lifetime;
      uint32_t	deferred_trx_expiration_window;
      uint32_t	max_transaction_delay;
      uint32_t	max_inline_action_size;
      uint16_t	max_inline_action_depth;
      uint16_t	max_authority_depth;
   };

   struct [[eosio::table("global"), eosio::contract("eosio.system")]] eosio_global_state : blockchain_parameters {
      uint64_t free_ram()const { return max_ram_size - total_ram_bytes_reserved; }

      uint64_t             max_ram_size = 64ll*1024 * 1024 * 1024;
      uint64_t             total_ram_bytes_reserved = 0;
      int64_t              total_ram_stake = 0;

      block_timestamp      last_producer_schedule_update;
      time_point           last_pervote_bucket_fill;
      int64_t              pervote_bucket = 0;
      int64_t              perblock_bucket = 0;
      uint32_t             total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
      int64_t              total_activated_stake = 0;
      time_point           thresh_activated_stake_time;
      uint16_t             last_producer_schedule_size = 0;
      double               total_producer_vote_weight = 0; /// the sum of all producer votes
      block_timestamp      last_name_close;
   };
   typedef eosio::singleton<"global"_n, eosio_global_state> global;

   /***
     * Returns the price of ram for given bytes
     */

   asset getRamCost(uint64_t ram_bytes)
   {
      RamInfo ramInfo(name("eosio"), name("eosio").value);
      auto ramData = ramInfo.find(S_RAM.raw());
      symbol coreSymbol = core_symbol;
      check(ramData != ramInfo.end(), "Could not get RAM info");

      uint64_t base = ramData->base.balance.amount;
      uint64_t quote = ramData->quote.balance.amount;
      return asset((((double)quote / base)) * ram_bytes, coreSymbol);
   }

   float getRamUtilization()
   {
      global gtable(name("eosio"), name("eosio").value);
      eosio_global_state _gstate = gtable.get();
      return (float)_gstate.total_ram_bytes_reserved / (float)_gstate.max_ram_size;
   }

   struct user_resources
   {
      name owner;
      asset net_weight;
      asset cpu_weight;
      int64_t ram_bytes = 0;

      bool is_empty() const { return net_weight.amount == 0 && cpu_weight.amount == 0 && ram_bytes == 0; }
      uint64_t primary_key() const { return owner.value; }
   };

   typedef eosio::multi_index<"userres"_n, user_resources> user_resources_table;

   asset getAccountCpu(name account)
   {
      user_resources_table _userres(name("eosio"), account.value);
      auto res_itr = _userres.find(account.value);
      return res_itr->cpu_weight;
   }
   asset getAccountNet(name account)
   {
      user_resources_table _userres(name("eosio"), account.value);
      auto res_itr = _userres.find(account.value);
      return res_itr->net_weight;
   }
   uint64_t getAccountRamBytes(name account)
   {
      user_resources_table _userres(name("eosio"), account.value);
      auto res_itr = _userres.find(account.value);
      return res_itr->ram_bytes;
   }

   struct account
   {
      asset balance;

      uint64_t primary_key() const { return balance.symbol.code().raw(); }
   };
   typedef eosio::multi_index<"accounts"_n, account> balance_table;

   asset getSYSBalance(name account)
   {
      balance_table _balance(name("eosio.token"), account.value);
      auto balance_itr = _balance.find(core_symbol.code().raw());
      if (balance_itr == _balance.end())
      {
         return asset(0, core_symbol);
      }
      else
      {
         return balance_itr->balance;
      }
   }

   struct [[eosio::table]] stake_info
   {
      asset amount;
      name staker;

      uint64_t primary_key() const { return amount.symbol.code().raw(); }
   };
   typedef eosio::multi_index<"stakeinfo"_n, stake_info> stakeinfo;

   name getStakerName(name account){
      stakeinfo _info(name("eosio.token"), account.value);
      auto stake_itr = _info.find(ore_symbol.code().raw());
      if(stake_itr == _info.end()){
         return name("");
      } else {
         return stake_itr->staker;
      }
   }

   asset getStakedAmount(name account){
      stakeinfo _info(name("eosio.token"), account.value);
      auto stake_itr = _info.find(ore_symbol.code().raw());
      if(stake_itr == _info.end()){
         return asset(0, ore_symbol);
      } else {
         return stake_itr->amount;
      }
   }

}; // namespace common