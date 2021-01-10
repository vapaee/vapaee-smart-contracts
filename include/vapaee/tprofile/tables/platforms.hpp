#include <vapaee/base/base.hpp>

// -- platforms --
// scope: contract.value
// row: represent one platform used in links globaly in the contract.

TABLE platform {
    uint64_t             id;  // auto-increment
    string            pname;  // platform name
    uint64_t        counter;  // global link counter

    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(pname);
    }
};

typedef eosio::multi_index<"platforms"_n, platform,
    indexed_by<"pname"_n, const_mem_fun<platform, checksum256, &platform::by_hash>>
> platforms;