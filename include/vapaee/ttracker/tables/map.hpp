#include <vapaee/base/base.hpp>

// -- maps --
// scope: contract
// row: this is a map and can be shered among several domes or even organizations

TABLE map {
    uint64_t           id;
    string          title; // title of this map
    string         config; // json containing all configuration relevant data te deploy the map
    uint64_t      creator; // profile id asociated with this map

    uint64_t primary_key() const {
        return id;
    }
};

typedef eosio::multi_index<"maps"_n, map> maps;
