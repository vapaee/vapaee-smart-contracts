#include <vapaee/base/base.hpp>

// -- domes --
// scope: organization
// row: a dome creates a context inside which a group of events occurs. Is a period of time (from starts to finish) in which geo-events can happend refering a specific target alive only in this dome.

TABLE dome {
    uint64_t           id;
    string          title;  // "Hemp Tracker" (abierto permanente), "Imperios-match-2423" (específico, abre y cierra y luego se borra)
    time_point_sec starts; 
    time_point_sec finish;  // if finish < starts then this means never finish
    string         config;  // json containing all organization-specific extra data thfor this dome (like, match-id, max-players, etc)

    uint64_t primary_key() const {
        return id;
    }
};

typedef eosio::multi_index<"domes"_n, dome> domes;
