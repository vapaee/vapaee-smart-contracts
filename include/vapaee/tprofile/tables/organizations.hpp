#include <vapaee/base/base.hpp>

// -- organizaton --
// scope: contract.value
// row: represent an organization created by a profile

TABLE organization {
    uint64_t               id;  // auto-increment
    string           org_name;  // platform name

    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(org_name);
    }
};

typedef eosio::multi_index<"orgs"_n, organization,
    indexed_by<"orgname"_n, const_mem_fun<organization, checksum256, &organization::by_hash>>
> organizations;

// -- membership --
// scope: organizaton.id
// row: each row is a member of the organization with his roles

TABLE membership {
    uint64_t     profile_id;  // profile id that has this membership
    vector<name> roles;       // list of roles

    uint64_t primary_key() const {
        return profile_id;
    }
};

typedef eosio::multi_index<"members"_n, membership> members;