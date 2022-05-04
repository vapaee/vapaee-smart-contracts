// -- withdraws --
// in some cases the the matured REX are not enough to cancel certain amount. For that we just list them
// in this table to be actually widrawn by the maintenance process when the REX are mature.

// scope: contract
// row: represents a future withdraw waiting REX to mature 

TABLE withdraw_table {
    uint64_t id;
    uint64_t loan_id;
    name to;
    asset quantity;
    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index<"withdraws"_n, withdraw_table> withdraws;
