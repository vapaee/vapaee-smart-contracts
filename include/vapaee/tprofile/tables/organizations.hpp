#include <vapaee/base/base.hpp>

// -- organizaton --
// scope: contract.value
// row: represent an organization created by a profile

TABLE organization {
    uint64_t               id;  // auto-increment
    string           org_name;  // platform name
    vector<uint64_t>  members;  // profile id list

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

// -- role --
// scope: organizaton.id
// row: represent a role a profile has inside an organization

TABLE role {
    uint64_t         id;  // auto-increment
    uint64_t profile_id;  // profile id that has this role
    name      role_name;  // unique role identifier

    uint64_t primary_key() const {
        return id;
    }

    uint64_t by_profile() const {
        return profile_id;
    }

    uint64_t by_role() const {
        return role_name.value;
    }
};

typedef eosio::multi_index<"roles"_n, role,
    indexed_by<"profile"_n, const_mem_fun<role, uint64_t, &role::by_profile>>,
    indexed_by<"role"_n,    const_mem_fun<role, uint64_t, &role::by_role>>
> roles;