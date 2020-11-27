#include <vapaee/base/base.hpp>

// -- profiles --
// scope: contract
// row: represent one profile on Telos indentified by a single alias (256bit hash of utf-8 string)

TABLE profile {
    uint64_t       id;  // auto generated, never changes
    name        owner;  // acount owner of this profile
    string      alias;  // string alias

    uint64_t   points; // 1 + weighted sum of the points of each link registered by the profile
    uint64_t kyclevel; // floor(log2(this.points)), same number as points but expresed in log scale, has no upper bound

    // functions ---------
    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(alias);
    }

    uint64_t owner_key() const {
        return owner.value;
    }

    string to_string() const {
        return std::to_string((int) id) + " - " + alias + " (" + owner.to_string() + ")";
    };
};


typedef eosio::multi_index<"profiles"_n, profile,
    indexed_by<"alias"_n, const_mem_fun<profile, checksum256, &profile::by_hash>>,
    indexed_by<"owner"_n, const_mem_fun<profile, uint64_t, &profile::owner_key>>
> profiles;