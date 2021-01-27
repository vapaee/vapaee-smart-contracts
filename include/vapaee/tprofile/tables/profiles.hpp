#include <vapaee/base/base.hpp>

// -- profiles --
// scope: contract
// row: represent one profile on Telos indentified by a single alias (256bit hash of utf-8 string)

TABLE profile_t {
    uint64_t              id;  // auto generated, never changes
    vector<name>      owners;  // list of owners of this profile
    name            contract;  // only apps can have a smart contract account. if contract is not null (value == 0) then this profile is an Application and represents an organization.
    string             alias;  // string alias
    uint64_t          points;  // 1 + weighted sum of the points of each link registered by the profile
    uint64_t        kyclevel;  // floor(log2(this.points)), same number as points but expresed in log scale, has no upper bound

    // functions ---------
    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(alias);
    }
};

typedef eosio::multi_index<"profiles"_n, profile_t,
    indexed_by<"alias"_n, const_mem_fun<profile_t, checksum256, &profile_t::by_hash>>
> profiles;
