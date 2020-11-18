#include <vapaee/base/base.hpp>

// -- links --
// scope: profile.id
// row: represent one external link for this profile.
// Ej: a facebook page, a youtube channel or an instagram profile.

const name LINK_PLATFORM_NAMES[5] = {
    "facebook"_n,
    "twitter"_n,
    "instagram"_n,
    "youtube"_n,
    "reddit"_n
};

TABLE link {
    uint64_t       link_id;  // auto-increment
    name          platform;  // facebook, youtube, twitter, instagram, etc
    string             url;  // link to an external platform or website page related with this profile
    string           proof;  // link to a specific publication in which apears the "alias" auto-generated random slug id.
    string           token;  // auto-generated proof token

    // functions ---------
    uint64_t primary_key() const {
        return link_id;
    }

    uint64_t by_platform() const {
        return platform.value;
    }

    // std::string to_string() const {
    //     return std::to_string((int) id) + " - " + username;
    // };
};

typedef eosio::multi_index<"links"_n, link,
    indexed_by<"platform"_n, const_mem_fun<link, uint64_t, &link::by_platform>>
> links;
