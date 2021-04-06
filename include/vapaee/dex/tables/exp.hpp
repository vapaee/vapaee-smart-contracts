#include "./_aux.hpp"

        
        // ------------------------------------
        // TABLE exp (permanent experience of the user) -----------
        // scope: contract
        TABLE exp_table {
            name owner;      
            asset exp;
            uint64_t primary_key() const { return owner.value; }
            uint64_t by_exp_key() const { return exp.amount; }
        };

        typedef eosio::multi_index< "exp"_n, exp_table,
            indexed_by<"exp"_n, const_mem_fun<exp_table, uint64_t, &exp_table::by_exp_key>>
        > exp;
        // ------------------------------------
        