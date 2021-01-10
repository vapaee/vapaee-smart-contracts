#include <vapaee/base/base.hpp>

// -- events --
// scope: organization
// row: each row is a geolocated-target-event under a specific dome on a specific map.

TABLE event {
    uint64_t           id;
    uint64_t       target;
    uint64_t      profile;
    name          account;  // this can be the profile owner account or the account of the org's smart contract using grant.access from telosprofile
    time_point_sec   date; 
    uint64_t         dome;  // dome identiies the dome containing all related events
    uint64_t          map;  // map_id identiies the map over which lat & lng coordenates take place
    asset             lat;  // "-4151916.057022739 LAT"  google coords system
    asset             lng;  // "-6253126.968753824 LONG" google coords system
    string       geometry;  // used only to paint an area on the global map when selected
    string         config;  // json containing all organization-specific extra data for this event (like unit-helth, etc)

    uint64_t primary_key() const {
        return id;
    }
};

typedef eosio::multi_index<"events"_n, event> events;
