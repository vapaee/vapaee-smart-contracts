// --- stats (currency) ---
// Standar table for all currency stats.

// scope: supply_code
// row: there's only 1 or 0 entry per scope. It represents a currency stat

// WARNING!! - STANDARD TABLE - DON'T CHANGE

TABLE currency_stats {
    eosio::asset           supply;
    eosio::asset           max_supply;
    name                   issuer;
    uint64_t primary_key()const { return supply.symbol.code().raw(); }
};

typedef eosio::multi_index< "stat"_n, currency_stats > stats;
