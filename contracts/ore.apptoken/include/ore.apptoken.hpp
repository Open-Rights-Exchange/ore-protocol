#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

#include <string>

using namespace eosio;
using namespace std;

using std::string;

/**
    * eosio.token contract defines the structures and actions that allow users to create, issue, and manage
    * tokens on EOSIO based blockchains.
    */
class [[eosio::contract("ore.apptoken")]] apptoken : public eosio::contract
{
public:
   apptoken( name receiver, name code, datastream<const char*> ds): contract(receiver, code, ds) {}

   /**
          * Allows `issuer` account to create a token in supply of `maximum_supply`. If validation is successful a new entry in statstable for token symbol scope gets created.
          *
          * @param issuer - the account that creates the token,
          * @param maximum_supply - the maximum supply set for the token created.
          *
          * @pre Token symbol has to be valid,
          * @pre Token symbol must not be already created,
          * @pre maximum_supply has to be smaller than the maximum supply allowed by the system: 1^62 - 1.
          * @pre Maximum supply must be positive;
          */
   ACTION create(const name &issuer,
                 const asset &maximum_supply);
   ACTION issue(const name &to, const asset &quantity, const string &memo);
   ACTION retire(const asset &quantity, const string &memo);
   ACTION transfer(const name &from,
                   const name &to,
                   const asset &quantity,
                   const string &memo);
   ACTION open(const name &owner, const symbol &symbol, const name &ram_payer);

   ACTION close(const name &owner, const symbol &symbol);

   static asset get_supply(const name &token_contract_account, const symbol_code &sym_code)
   {
      stats statstable(token_contract_account, sym_code.raw());
      const auto &st = statstable.get(sym_code.raw());
      return st.supply;
   }

   static asset get_balance(const name &token_contract_account, const name &owner, const symbol_code &sym_code)
   {
      accounts accountstable(token_contract_account, owner.value);
      const auto &ac = accountstable.get(sym_code.raw());
      return ac.balance;
   }

   using create_action = eosio::action_wrapper<"create"_n, &apptoken::create>;
   using issue_action = eosio::action_wrapper<"issue"_n, &apptoken::issue>;
   using retire_action = eosio::action_wrapper<"retire"_n, &apptoken::retire>;
   using transfer_action = eosio::action_wrapper<"transfer"_n, &apptoken::transfer>;
   using open_action = eosio::action_wrapper<"open"_n, &apptoken::open>;
   using close_action = eosio::action_wrapper<"close"_n, &apptoken::close>;

private:
   struct [[eosio::table]] account
   {
      asset balance;

      uint64_t primary_key() const { return balance.symbol.code().raw(); }
   };

   struct [[eosio::table]] currency_stats
   {
      asset supply;
      asset max_supply;
      name issuer;

      uint64_t primary_key() const { return supply.symbol.code().raw(); }
   };

   typedef eosio::multi_index<"accounts"_n, account> accounts;
   typedef eosio::multi_index<"stat"_n, currency_stats> stats;

   void sub_balance(const name &owner, const asset &value);
   void add_balance(const name &owner, const asset &value, const name &ram_payer);
};
