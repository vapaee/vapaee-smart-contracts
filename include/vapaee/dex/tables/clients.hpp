#include "./_aux.hpp"

        
        // ------------------------------------
        // TABLE clients (registered Telos DEX UI) -----------
        // scope: contract
        TABLE clients_table {
            uint64_t id;
            name admin;    // whoever register or manage this UI
            name receiver; // where to send earnings from fees
            string params; // text to add in all transfers' memos (like an internal id when you deposit in an exchange)
            string title;
            string website;
            string brief;
            string banner;    // big wide image
            string thumbnail; // small image
            time_point_sec date;
            uint64_t primary_key() const { return id; }
            uint64_t by_receiver_key() const { return receiver.value; }
        };

        typedef eosio::multi_index< "clients"_n, clients_table,
            indexed_by<"receiver"_n, const_mem_fun<clients_table, uint64_t, &clients_table::by_receiver_key>>
        > clients;
        // ------------------------------------
        