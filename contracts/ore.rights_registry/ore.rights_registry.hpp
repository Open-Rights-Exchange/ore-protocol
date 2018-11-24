/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include "../ore_types/ore_types.hpp"
using namespace eosio;
using namespace std;

class rights_registry : public contract
{
  public:
    //@abi table rights i64
    struct right_reg
    {
        uint64_t id;
        string right_name;
        account_name owner;
        vector<ore_types::endpoint_url> urls;
        vector<account_name> issuer_whitelist;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(right_reg, (id)(right_name)(owner)(urls)(issuer_whitelist))
    };

    typedef multi_index<N(rights), right_reg> right_registration_index;

  public:
    rights_registry(account_name self)
        : contract(self) {}

    void upsertright(account_name owner, string &right_name, vector<ore_types::endpoint_url> urls, vector<account_name> issuer_whitelist);
    void deleteright(account_name owner, string &right_name);

    inline static uint64_t hashStr(const string &strkey)
    {
        return hash<string>{}(strkey);
    }

    right_reg find_right_by_name(string right_name)
    {
        right_registration_index _endpoints(_self, _self);

        auto rightitr = _endpoints.find(hashStr(right_name));

        if (rightitr == _endpoints.end())
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