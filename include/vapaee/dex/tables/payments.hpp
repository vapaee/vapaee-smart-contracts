// --- payments ---
// Some actions on DAO DEX may have some costs This table stores temporaly the payments made by the user.

// scope: concept ("ballor", "addtoken")
// row: account who has paid the fee for a tha concept

TABLE payments_table {
    name payer;
    uint64_t primary_key() const { return payer.value; }
};

typedef eosio::multi_index< "payments"_n, payments_table > payments;
