#include "./_aux.hpp"

        
        // ------------------------------------
        // TABLE markets
        // scope: contract
        TABLE markets_table {
            uint64_t id;
            name table;     // xxx.zzz (do not asume it is unique)
            symbol_code commodity;
            symbol_code currency;
            uint64_t primary_key() const { return id; }
            uint64_t by_table_key() const { return table.value; }
            uint64_t by_commodity_key() const { return commodity.raw(); }
            uint64_t by_currency_key() const { return currency.raw(); }
        };

        typedef eosio::multi_index< "markets"_n, markets_table,
            indexed_by<"table"_n, const_mem_fun<markets_table, uint64_t, &markets_table::by_table_key>>,
            indexed_by<"commodity"_n, const_mem_fun<markets_table, uint64_t, &markets_table::by_commodity_key>>,
            indexed_by<"currency"_n, const_mem_fun<markets_table, uint64_t, &markets_table::by_currency_key>>
        > markets;
        // ------------------------------------
        