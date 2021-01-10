#include <vapaee/base/base.hpp>

// -- targets --
// scope: organization
// row: this is a target for a specific dome but can coexists in different maps (meaning that events for this target need to be in the same dome but can be different maps).

TABLE target {
    uint64_t             id;
    uint64_t           dome;
    string            title;
    time_point_sec  created;
    string           config; // json descrbeing the target with organization-specific data
                              // in case of target having a fized known position, then map lat & lng can be used.
    uint64_t            map;  // map_id identiies the map over which lat & lng coordenates take place
    asset               lat;  // "-4151916.057022739 LAT"  google coords system
    asset               lng;  // "-6253126.968753824 LONG" google coords system

    uint64_t primary_key() const {
        return id;
    }
};

typedef eosio::multi_index<"targets"_n, target> targets;
