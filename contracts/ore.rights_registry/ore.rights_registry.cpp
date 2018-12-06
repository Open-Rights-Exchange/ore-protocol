#include "ore.rights_registry.hpp"

using namespace eosio;

// transfer action
void rights_registry::upsertright(name owner, string &right_name, vector<ore_types::endpoint_url> urls, vector<name> issuer_whitelist)
{
    require_auth(owner);

    //right_registration_index right_registration(_self, _self);

    auto itr = rightsindex.find(hashStr(right_name));

    if (itr == rightsindex.end())
    {
        rightsindex.emplace(owner, [&](auto &end) {
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
        rightsindex.modify(itr, owner, [&](auto &end) {
            end.urls = urls;
            end.issuer_whitelist = issuer_whitelist;
        });
        print("modified");
    }
}

void rights_registry::deleteright(name owner, string &right_name)
{
    require_auth(owner);

    //right_registration_index right_registration(_self, _self);

    auto itr = rightsindex.find(hashStr(right_name));

    eosio_assert(itr != rightsindex.end(), "There is no right with that name");
  
    rightsindex.erase(itr);
}

EOSIO_DISPATCH(rights_registry, (upsertright)(deleteright))
