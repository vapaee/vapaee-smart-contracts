#include <vapaee/base/base.hpp>

// -- platforms --
// scope: contract.value
// row: represent one platform used in links globaly in the contract.

TABLE platform {
    name id;
    uint64_t counter;

    uint64_t primary_key() const {
        return id.value;
    }
};

typedef eosio::multi_index<"platforms"_n, platform> platforms;

// -- links --
// scope: profile.id
// row: represent one external link for this profile.
// E.g.: a facebook page, a youtube channel or an instagram profile.

TABLE link {
    uint64_t       link_id;  // auto-increment
    name          platform;  // facebook, youtube, twitter, instagram, etc
    string             url;  // link to an external platform or website page related with this profile
    string           proof;  // link to a specific publication in which apears the "alias" auto-generated random slug id.
    string           token;  // auto-generated proof token

    uint64_t primary_key() const {
        return link_id;
    }

    uint64_t by_platform() const {
        return platform.value;
    }
};

typedef eosio::multi_index<"links"_n, link,
    indexed_by<"platform"_n, const_mem_fun<link, uint64_t, &link::by_platform>>
> links;
