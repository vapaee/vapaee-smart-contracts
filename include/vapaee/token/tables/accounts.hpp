// -- account --
// This is the standard token account structure.

// DO NOT CHANGE

// scope: owner
// row: the balances for the owner, accesing by symbol_code


TABLE account_table {
    asset    balance;
    uint64_t primary_key()const { return balance.symbol.code().raw(); }
};

typedef eosio::multi_index<"accounts"_n, account_table> accounts;
