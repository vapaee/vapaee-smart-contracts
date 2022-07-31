// --- issuers ---
// Native Vapaée tokens can have more than one issuer. In this table, we store the list of issuers that a token has along with the maximum supply this account can issue.

// scope: token (supply_code)
// row: represent an issuer of the token

TABLE issuers_table {
    name issuer;
    asset max;
    uint64_t primary_key() const { return issuer.value; }
};

typedef eosio::multi_index< "issuers"_n, issuers_table > issuers;
