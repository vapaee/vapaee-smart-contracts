// -- targets --
// scope: season.id
// row: this is a target for a specific season, events can occur
// in different maps but be in the same target.

struct [[eosio::table]] target_t {
    uint64_t             id;
    uint64_t         season;
    string            title;
    time_point_sec  created;
    string           config;  // json descrbeing the target with organization-specific data
                              // in case of target having a fized known position, then map lat & lng can be used.
    uint64_t            map;  // map over which lat & lng coordenates take place
    asset               lat;  // "-4151916.057022739 LAT"  google coords system
    asset               lng;  // "-6253126.968753824 LONG" google coords system

    vector<uint64_t> whitelist;  // profile ids that can add events to this target

    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(title);
    }
};

typedef eosio::multi_index<"targets"_n, target_t,
    indexed_by<"title"_n, const_mem_fun<target_t, checksum256, &target_t::by_hash>>
> targets;
