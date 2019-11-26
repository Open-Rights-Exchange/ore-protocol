#include "../include/ore.rights_registry.hpp"

using namespace eosio;

// transfer action
ACTION rights_registry::upsertright(name owner, string &right_name, vector<oretypes::endpoint_url> urls, vector<name> issuer_whitelist)
{
    require_auth(owner);

    auto itr = _rights.find(hashStr(right_name));

    if (itr == _rights.end())
    {
        _rights.emplace(owner, [&](auto &end) {
            end.id = hashStr(right_name);
            end.right_name = right_name;
            end.owner = owner;
            end.urls = urls;
            end.issuer_whitelist = issuer_whitelist;
        });

        print("action:upsertright Right: " + right_name + " added:" + " by: " + owner.to_string() + "\n");

    }
    else
    {
        string msg = "The account " + owner.to_string() + " is not the owner of the right " + right_name + " and cannot modify it.";
        eosio_assert(itr->owner == owner, msg.c_str());

        _rights.modify(itr, owner, [&](auto &end) {
            end.urls = urls;
            end.issuer_whitelist = issuer_whitelist;
        });

        print("action:upsertright Right: " + right_name + " modified by: " + owner.to_string() + "\n");
    }
}

ACTION rights_registry::deleteright(name owner, string &right_name)
{
    require_auth(owner);

    auto itr = _rights.find(hashStr(right_name));

    string msg = "The right " + right_name + " doesn't exist ";

    eosio_assert(itr != _rights.end(), msg.c_str());
    _rights.erase(itr);
}

EOSIO_DISPATCH(rights_registry, (upsertright)(deleteright))
