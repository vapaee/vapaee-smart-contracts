#include <vapaee/base/base.hpp>

// -- profiles --
// scope: contract
// row: represent one profile on Telos indentified by a single alias (256bit hash of utf-8 string)

TABLE profile {
    uint64_t           id;  // auto generated, never changes
    name            owner;  // acount owner of this profile
    std::string     alias;  // string alias

    // functions ---------
    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(owner.to_string() + alias);
    }

    uint64_t owner_key() const {
        return owner.value;
    }

    std::string to_string() const {
        return std::to_string((int) id) + " - " + alias + " (" + owner.to_string() + ")";
    };
};


typedef eosio::multi_index<"profiles"_n, profile,
    indexed_by<"alias"_n, const_mem_fun<profile, checksum256, &profile::by_hash>>,
    indexed_by<"owner"_n, const_mem_fun<profile, uint64_t, &profile::owner_key>>
> profiles;