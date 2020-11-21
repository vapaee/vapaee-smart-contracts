#include <vapaee/base/base.hpp>

// -- platforms --
// scope: contract.value
// row: represent one platform used in links globaly in the contract.

TABLE platform {
    uint64_t      id;  // auto-increment
    string     pname;  // platform name
    uint64_t counter;  // global link counter

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

// -- links --
// scope: profile.id
// row: represent one external link for this profile.
// E.g.: a facebook page, a youtube channel or an instagram profile.

TABLE link {
    uint64_t       link_id;  // auto-increment
    uint64_t   platform_id;  // relational id to row in platform table
    string             url;  // link to an external platform or website page related with this profile
    string           proof;  // link to a specific publication in which apears the "alias" auto-generated random slug id.
    string           token;  // auto-generated proof token

    uint64_t primary_key() const {
        return link_id;
    }

    uint64_t by_platform() const { 
        return platform_id;
    }
};

typedef eosio::multi_index<"links"_n, link,
    indexed_by<"platform"_n, const_mem_fun<link, uint64_t, &link::by_platform>>
> links;
