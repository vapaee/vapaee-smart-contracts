// -- recipient --
// 

// scope: contract
// row: 

struct recipient_info {
    asset part;
    string target;    // It can refer to one of these options:
                      // -1 - not valid
                      //  0 - no specified yet
                      //  1 - it is a name and exists the account -> Telos account
                      //  2 - it is a number -> PayHub(id)
                      //  2 - it is a "<symbol_code>-<name>" -> REX pool (token & pool_id)
                      //  4 - it is a simple string -> must exist a (vip) name -> PayHub 
    std::string to_string() const {
        return part.to_string() + ": " + target;
    }                      
};

TABLE payhubs_table {
    uint64_t id;
    string alias;
    name admin;
    std::vector<recipient_info>recipients;

    uint64_t primary_key() const { return id; }
    checksum256 by_alias() const { 
        return vapaee::utils::hash(alias);
    }
};

typedef eosio::multi_index<"payhubs"_n, payhubs_table,
    indexed_by<"alias"_n, const_mem_fun<payhubs_table, checksum256, &payhubs_table::by_alias>>
> payhubs;