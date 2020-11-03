#include <vapaee/base/base.hpp>

// -- profiles --
// scope: contract
// row: represent one profile on Telos indentified by a single alias (256bit string 6bit-endoded number)

TABLE profile {
    uint64_t            id;  // auto generated
    slug             alias;  // 256bit number id with an encoded string in 6 bits
    name             owner;  // acount owner of this profile

    // lista de data
    // lista

    // functions ---------
    uint64_t primary_key() const {
        return id;
    }

    uint128_t alias_key() const {
        return alias.to128bits();
    }

    uint64_t owner_key() const {
        return owner.value;
    }

    std::string to_string() const {
        return std::to_string((int) id) + " - " + alias.to_string() + " (" + owner.to_string() + ")";
    };
};


typedef eosio::multi_index<"profiles"_n, profile,
    indexed_by<"alias"_n, const_mem_fun<profile, uint128_t, &profile::alias_key>>,
    indexed_by<"owner"_n, const_mem_fun<profile, uint64_t, &profile::owner_key>>
> profiles;