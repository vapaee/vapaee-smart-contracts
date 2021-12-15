// --- tokenevents ---
// each token can emit events to be consumed by other contracts. 

// scope: token_symbol
// row: a token event callback

TABLE tokenevents_table {
    name event;
    name receptor;
    uint64_t primary_key() const { return event.value; }
};

typedef eosio::multi_index< "tokenevents"_n, tokenevents_table > tokenevents;
