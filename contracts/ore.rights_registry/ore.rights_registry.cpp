#include "ore.rights_registry.hpp"

using namespace eosio;

// transfer action
void rights_registry::upsertright(account_name owner, string &right_name, vector<ore_types::endpoint_url> urls, vector<account_name> issuer_whitelist)
{
    require_auth(owner);

    right_registration_index right_registration(_self, _self);

    auto itr = right_registration.find(hashStr(right_name));

    if (itr == right_registration.end())
    {
        right_registration.emplace(owner, [&](auto &end) {
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
        right_registration.modify(itr, owner, [&](auto &end) {
            end.urls = urls;
            end.issuer_whitelist = issuer_whitelist;
        });
        print("modified");
    }
}

void rights_registry::deleteright(account_name owner, string &right_name)
{
    require_auth(owner);

    right_registration_index right_registration(_self, _self);

    auto itr = right_registration.find(hashStr(right_name));

    eosio_assert(itr != right_registration.end(), "There is no right with that name");
  
    right_registration.erase(itr);
}

EOSIO_ABI(rights_registry, (upsertright)(deleteright))
