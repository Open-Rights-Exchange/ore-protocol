/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include "eosiolib/eosio.hpp"
#include "../ore_types/ore_types.hpp"
using namespace eosio;
using namespace std;

class [[eosio::contract("ore.rights_registry")]] rights_registry : public contract
{
  public:
    using contract::contract;
    rights_registry( name receiver, name code, datastream<const char*> ds): contract(receiver, code, ds), rightsindex(receiver, receiver.value){}

    struct [[eosio::table]] right_reg
    {
        uint64_t id;
        string right_name;
        name owner;
        vector<ore_types::endpoint_url> urls;
        vector<name> issuer_whitelist;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(right_reg, (id)(right_name)(owner)(urls)(issuer_whitelist))
    };

    typedef eosio::multi_index<"rights"_n, right_reg>right_registration_index;
    
  public:
    right_registration_index rightsindex;

    [[eosio::action]]
    void upsertright(name owner, string &right_name, vector<ore_types::endpoint_url> urls, vector<name> issuer_whitelist);
    
    [[eosio::action]]
    void deleteright(name owner, string &right_name);

    inline static uint64_t hashStr(const string &strkey)
    {
        return hash<string>{}(strkey);
    }

    right_reg find_right_by_name(string right_name)
    {
       // right_registration_index endpoints(_self, _self);

        auto rightitr = rightsindex.find(hashStr(right_name));

        if (rightitr == rightsindex.end())
        {
            return right_reg{0};
        }

        return right_reg{
            rightitr->id,
            rightitr->right_name,
            rightitr->owner,
            rightitr->urls,
            rightitr->issuer_whitelist};
    }
};