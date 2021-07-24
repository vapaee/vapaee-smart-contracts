// -- events --
// scope: season
// row: each row is a geolocated-target-event under a specific season on a specific map.

struct [[eosio::table]] event_t {
    uint64_t           id;
    uint64_t       target;
    uint64_t      profile;  // event creator profile
    time_point_sec   date;
    uint64_t          map;  // map over which lat & lng coordenates take place
    asset             lat;  // "-4151916.057022739 LAT"  google coords system
    asset             lng;  // "-6253126.968753824 LONG" google coords system
    string       geometry;  // used only to paint an area on the global map when selected
    string         config;  // json containing all organization-specific extra data for this event (like unit-helth, etc)

    uint64_t primary_key() const {
        return id;
    }
};

typedef eosio::multi_index<"events"_n, event_t> events;
