#include "ore.rights_registry.hpp"

using namespace eosio;

// transfer action
ACTION rights_registry::upsertright(name owner, string &right_name, vector<ore_types::endpoint_url> urls, vector<name> issuer_whitelist)
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

        print("emplaces");
    }
    else
    {
        eosio_assert(itr->owner == owner, "You are not the issuer of the existing right name. Update canceled!");
        _rights.modify(itr, owner, [&](auto &end) {
            end.urls = urls;
            end.issuer_whitelist = issuer_whitelist;
        });
        print("modified");
    }
}

ACTION rights_registry::deleteright(name owner, string &right_name)
{
    require_auth(owner);

    auto itr = _rights.find(hashStr(right_name));

    eosio_assert(itr != _rights.end(), "There is no right with that name");
  
    _rights.erase(itr);
}

EOSIO_DISPATCH(rights_registry, (upsertright)(deleteright))
