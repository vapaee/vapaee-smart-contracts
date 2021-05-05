#include <vapaee/base/base.hpp>

// -- links --
// scope: profile.id
// row: represent one external link for this profile.
// E.g.: a facebook page, a youtube channel or an instagram profile.

#define MAX_WITNESS 5

TABLE link_t {
    uint64_t id;          // auto-increment
    uint64_t platform_id; // relational id to row in platform table
    string   url;         // link to an external platform or website page related with this profile
    string   proof;       // link to a specific publication in which apears the "alias" auto-generated random slug id.
    string   token;       // auto-generated proof token

    uint64_t points;  // total sum of the profile scores witnessing this link
    
    // list of {points, profile_id} that witnessed this link
    vector<tuple<uint64_t, uint64_t>> witnesses;

    uint64_t primary_key() const {
        return id;
    }

    uint64_t by_platform() const { 
        return platform_id;
    }
};

typedef eosio::multi_index<"links"_n, link_t,
    indexed_by<"platform"_n, const_mem_fun<link_t, uint64_t, &link_t::by_platform>>
> links;
