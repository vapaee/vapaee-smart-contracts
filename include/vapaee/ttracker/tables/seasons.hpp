#include <vapaee/base/base.hpp>

// -- seasons --
// scope: contract
// row: a period of time (from starts to finish) in which geo-events can
// happen refering a specific target alive only in this season.

TABLE season_t {
    uint64_t           id;
    string          title;  // unique string
    time_point_sec  start; 
    time_point_sec finish;  // if finish < start then this means never finish
    string         config;  // json containing all organization-specific extra data for this season (like, match-id, max-players, etc)

    vector<uint64_t> tc_whitelist; // target creator whitelist, prof ids that can add targets

    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(title);
    }
};

typedef eosio::multi_index<"seasons"_n, season_t,
    indexed_by<"title"_n, const_mem_fun<season_t, checksum256, &season_t::by_hash>>
> seasons;

