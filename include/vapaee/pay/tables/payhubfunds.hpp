// -- payhubfunds --
// 

// scope: contract
// row: 


TABLE payhubfunds_table {
    uint64_t id;
    asset balance;
    uint64_t payhub;
    time_point_sec in;
    time_point_sec out;

    string to_string() const {
        return std::to_string((unsigned long) payhub) + ":" + balance.symbol.code().to_string();
    }

    uint64_t primary_key()  const { return id; }
    uint32_t last_in()      const { return in.sec_since_epoch(); }
    uint32_t last_out()     const { return out.sec_since_epoch(); }
    uint64_t by_payhub()    const { return payhub; }
    uint64_t by_oldest()    const { return (uint64_t) (time_point_sec::maximum().sec_since_epoch() - (last_in()-last_out())); }
    checksum256 by_pocket() const { 
        string str = to_string();
        return vapaee::utils::hash(str);
    }
};

typedef eosio::multi_index<"payhubfunds"_n, payhubfunds_table,
    indexed_by<"payhub"_n, const_mem_fun<payhubfunds_table, uint64_t, &payhubfunds_table::by_payhub>>,
    indexed_by<"oldest"_n, const_mem_fun<payhubfunds_table, uint64_t, &payhubfunds_table::by_oldest>>,
    indexed_by<"pocket"_n, const_mem_fun<payhubfunds_table, checksum256, &payhubfunds_table::by_pocket>>
> payhubfunds;