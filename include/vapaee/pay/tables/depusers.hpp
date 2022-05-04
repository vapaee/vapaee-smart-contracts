// --- depusers ---
// All users the interact with the PayHub will be registered in this table.

// scope: contract
// row: represent a user that is a member of the PayHub system

TABLE depusers_table {
    name account;
    uint64_t primary_key() const { return account.value; }
};

typedef eosio::multi_index< "depusers"_n, depusers_table > depusers;
