// --- swaps ---
// Ths is a temporary table to hold in-proccess swaps.

// scope: contract
// row: represent the last of in-proccess swap of a trader


TABLE swaps_table {
    uint64_t id;
    name trader;       // user who swaps
    asset in;          // amount of incoming tokens 
    asset out;         // amount of minimun tolerated outgoing tokens 
    asset got;         // amount of final outgoing tokens

    // list of tuples of trades (asset, asset)
    vector<std::tuple<asset, asset>> trades;


    uint64_t primary_key()const { return id; }
};

typedef eosio::multi_index< "swaps"_n, swaps_table > swaps;

/*

// TODO: do we need this table?
THIS TABLE IS NOT IN USE YET

*/