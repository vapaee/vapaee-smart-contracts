#include <vapaee/base/base.hpp>

// -- links --
// scope: profile.id
// row: represent one external link for this profile.
// Ej: a facebook page, a youtube channel or an instagram profile.

TABLE link {
    name          platform;  // facebook, youtube, twitter, instagram, etc
    string             url;  // link to an external platform or website page related with this profile
    string           proof;  // link to a specific publication in which apears the "alias" auto-generated random slug id.
    slug             alias;  // auto-generated random slug id

    // functions ---------
    uint16_t primary_key() const {
        return platform.value;
    }

    uint128_t alias_key() const {
        return alias.to128bits();
    }

    // std::string to_string() const {
    //     return std::to_string((int) id) + " - " + username;
    // };
};

typedef eosio::multi_index<"links"_n, link,
    indexed_by<"alias"_n, const_mem_fun<link, uint128_t, &link::alias_key>>
> links;
