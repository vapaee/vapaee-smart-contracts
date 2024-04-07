// --- knowntokens ---
// Holdes information about all knowntokens known by this contract, whether they are wrapped knowntokens or foreign knowntokens.
// Al accepted knowntokens must be registered in telosmaindex

// scope: vapaeeknowntokens
// row: indicates the origin of a certain token.

TABLE knowntokens_table {
    asset  supply;
    name   account;
    uint64_t primary_key() const { return supply.symbol.code().raw(); }
};

typedef eosio::multi_index< "knowntokens"_n, knowntokens_table > knowntokens;
