#include "./_aux.hpp"

        // TABLE userorders -----------
        // scope: owner
        TABLE userorders_table {
            uint64_t market;
            string table;
            vector<uint64_t> ids;
            uint64_t primary_key() const { return market; }
        };
        typedef eosio::multi_index< "userorders"_n, userorders_table > userorders;
        // ------------------------------------