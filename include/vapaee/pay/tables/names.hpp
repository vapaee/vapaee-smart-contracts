// -- names --
// 

// scope: contract
// row: 


TABLE names_table {
    uint64_t id;
    name owner;
    string alias;
    uint64_t primary_key() const { return id; }
    checksum256 by_alias() const { 
        return vapaee::utils::hash(alias);
    }    
};

typedef eosio::multi_index<"names"_n, names_table,
    indexed_by<"alias"_n, const_mem_fun<names_table, checksum256, &names_table::by_alias>>
> names;
