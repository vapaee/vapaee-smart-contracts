// --- dump ---
// Some NFTs are needed to store specific data representing the state of the NFT within another system. This performs similar to the text table, but is aimed to store NFTs temporal data in a free dump format.

// scope: contract
// row: represents a dump extension for any given NFT

TABLE dump_table {
    uint64_t id;
    string   dump; // free memdump containing dapp specific information related to

    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index<"dumps"_n, dump_table> dumps;