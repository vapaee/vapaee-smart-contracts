// TODO: (NFT) esta tabla hay que partirla en dos: 
// - una para listar todos los conceptos y nada más (para el estandar)
// - otra para los datos concreto del cocepto (lo que guarda esta tabla actualmente)
// - label deberías ser el id en ambas tablas

// -- concept --
// Each dapp can register several concepts which will later be instantiated as nfts and or nft-containers (nftc)
// At leas one of is_contaier or is_nft must be true.
// scope: contract
// row: represents a concept that you can read in label

TABLE concept_table {
    uint64_t id;
    name     label;                       // Label that identifies it within the dapp_id namespace
    uint64_t dapp_id;                     // Only a dapp can define concepts
    bool     is_container;                // Indicates whether the concept is a container for NTFs or not.
    bool     is_nft;                      // Indicates whether the concept is an NFT in itself or not.
    uint64_t readme;                      // free format text (html or MarkDown suggested) for public reading stored in texts table
    vector<tuple<name, uint64_t>> data;   // List of text properties stored in texts table

    uint64_t primary_key() const { return id; }
    uint64_t by_label() const { return label.value; }
    uint64_t by_dapp() const { return dapp_id; }
};

typedef eosio::multi_index<"concepts"_n, concept_table,
    indexed_by<"label"_n, const_mem_fun<concept_table, uint64_t, &concept_table::by_label>>,
    indexed_by<"dapp"_n, const_mem_fun<concept_table, uint64_t, &concept_table::by_dapp>>
> concepts;