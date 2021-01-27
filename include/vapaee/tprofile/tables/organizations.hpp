#include <vapaee/base/base.hpp>

// -- organizaton --
// scope: contract.value
// row: represent an organization created by a profile

TABLE organization_t {
    uint64_t                   id;  // auto-increment
    string               org_name;  // platform name
    uint64_t              profile;  // profile id
    asset                  points;  // asbtract field to acumulate some asset globally. 
    asset                 credits;  // asbtract field to acumulate some asset globally. 
    asset                 rewards;  // asbtract field to acumulate some asset globally. 
    asset                   trust;  // asbtract field to acumulate some asset globally. 
    asset                     rep;  // asbtract field to acumulate some asset globally. 

    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(org_name);
    }
};

typedef eosio::multi_index<"orgs"_n, organization_t,
    indexed_by<"orgname"_n, const_mem_fun<organization_t, checksum256, &organization_t::by_hash>>
> organizations;

