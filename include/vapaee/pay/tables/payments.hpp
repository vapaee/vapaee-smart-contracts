// -- payments --
// 
// scope: contract
// row: a pending payment

#include "../datatypes/payhub_target.hpp"

TABLE payments_table {
    uint64_t id;
    asset quantity;
    payhub_target target;
    string memo;
    time_point_sec in;

    string to_string() const {
        // return string("target.alias") + ":" + quantity.symbol.code().to_string();
        return target.alias + ":" + quantity.symbol.code().to_string();
    }

    uint64_t primary_key()  const { return id; }
    uint32_t last_in()      const { return in.sec_since_epoch(); }
    uint64_t by_payhub()    const { return target.payhub; }
    checksum256 hash() const { 
        string str = to_string();
        return vapaee::utils::hash(str);
    }
};

typedef eosio::multi_index<"payments"_n, payments_table,
    indexed_by<"hash"_n, const_mem_fun<payments_table, checksum256, &payments_table::hash>>
> payments;