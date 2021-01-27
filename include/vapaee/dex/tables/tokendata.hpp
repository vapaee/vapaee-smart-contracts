#include "./_aux.hpp"

        // TABLE sellorders (registered currency) -----------
        // scope: symbol_code
        TABLE tokendata_table {
            uint64_t id;
            string text;
            string link;
            name shownas;
            time_point_sec date;
            uint64_t primary_key() const { return id; }
            uint64_t by_shownas_key() const { return shownas.value; }
        };

        typedef eosio::multi_index< "tokendata"_n, tokendata_table,
            indexed_by<"shownas"_n, const_mem_fun<tokendata_table, uint64_t, &tokendata_table::by_shownas_key>>
        > tokendata;
        // ------------------------------------
        