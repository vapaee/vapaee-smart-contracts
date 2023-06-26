// --- converters (UX) ---
// Each market can have more than one converter.
// Every converter is registereed in this table


// auxiliar struct converter_state to express the state of a converter
struct converter_state {
    asset currency;  // currency liquidity
    asset commodity; // commodity liquidity
    asset price;     // price of commodity expressed in currency
    asset inverse;   // price of currency expressed in commodity 
    asset fee;       // percent
};


// scope: contract
// row: represent a interface client that is using the DEX DAO system

TABLE converters_table {
    uint64_t id;
    uint64_t market_id;
    string market_name;
    name admin;     // whoever register or manage this UI
    name converter; // account in which the converter is located
    time_point_sec date;
    converter_state state;
    uint64_t primary_key() const { return id; }
    uint64_t by_market_key() const { return market_id; }
};

typedef eosio::multi_index< "converters"_n, converters_table,
    indexed_by<"market"_n, const_mem_fun<converters_table, uint64_t, &converters_table::by_market_key>>
> converters;
