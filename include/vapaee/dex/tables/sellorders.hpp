#include "./_aux.hpp"

        // TABLE sellorders -----------
        // scope: market-id
        TABLE sell_order_table {
            uint64_t id;
            uint64_t client;
            name owner;
            asset price;    // currency_P  - how much P per A unit
            asset inverse;  // commodity_A - inverse price 
            asset total;    // currency_P  - how much P you want
            asset selling;  // commodity_A - how much A you are selling
            uint64_t primary_key() const { return id; }
            uint64_t by_price_key() const { return price.amount; }
            // uint64_t by_inverse_key() const { return inverse.amount; } 
            uint64_t by_owner_key() const { return owner.value; }
            uint64_t by_total_key() const { return total.amount; }
        };

        typedef eosio::multi_index< "sellorders"_n, sell_order_table,
            indexed_by<"price"_n, const_mem_fun<sell_order_table, uint64_t, &sell_order_table::by_price_key>>
        > sellorders;
        // ------------------------------------