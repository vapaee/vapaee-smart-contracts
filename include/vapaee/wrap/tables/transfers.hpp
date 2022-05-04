// -- transfers --
// some times the contract needs to perform a lot of transfers and those transfers trigger more actions.
// in order to avoid falling into a comppute-heavy-transaction, we agendate the transfers for tuture excecution

// scope: contract
// row: represents a transfer that must be done which funds are already in hands of the contract

TABLE transfer_table {
    uint64_t id;
    name to;
    asset quantity;
    string memo;
    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index<"transfers"_n, transfer_table> transfers;
