// -- recipient --
// 

// scope: contract
// row:


#include "../datatypes/recipient_info.hpp"

TABLE payhubs_table {
    uint64_t id;
    string alias;
    checksum256 hash;
    name admin;
    std::vector<recipient_info>recipients;
    name billing_to;             // where to send a copy of the invoice
    symbol_code main_pocket;     // main pocket for this payhub

    uint64_t primary_key() const { return id; }
    checksum256 by_alias() const { 
        return vapaee::utils::hash(alias);
    }
    std::string to_string() const {
        return 
            string("payhubs: ") + std::to_string(id) +
            " alias: " + alias +
            " admin: " + admin.to_string() +
            " recipients: " + std::to_string(recipients.size()) +
            " billing_to: " + billing_to.to_string() +
            " main_pocket: " + main_pocket.to_string();
    }
};

typedef eosio::multi_index<"payhubs"_n, payhubs_table,
    indexed_by<"alias"_n, const_mem_fun<payhubs_table, checksum256, &payhubs_table::by_alias>>
> payhubs;