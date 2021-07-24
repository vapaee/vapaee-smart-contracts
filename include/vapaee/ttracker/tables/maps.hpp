// -- maps --
// scope: contract
// row: this is a map and can be shered among several seasons or even organizations

struct [[eosio::table]] map_t {
    uint64_t           id;
    string          title; // title of this map
    string         config; // json containing all configuration relevant data te deploy the map
    uint64_t      creator; // profile id asociated with this map

    uint64_t primary_key() const {
        return id;
    }

    checksum256 by_hash() const { 
        return vapaee::utils::hash(title);
    }
};

typedef eosio::multi_index<"maps"_n, map_t,
    indexed_by<"title"_n, const_mem_fun<map_t, checksum256, &map_t::by_hash>>
> maps;
