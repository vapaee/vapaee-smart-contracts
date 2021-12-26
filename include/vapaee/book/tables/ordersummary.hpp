// --  ordersummary -----------
// This table is a summary of the order tables for a given market.
// This is very usefull to present fast agregated information (supply - demand) about a market order-book. 

// scope: contract
// row: represents a market's orders summary

struct booksummary {
    asset total;
    int orders;
    int ascurrency;
};

TABLE ordersummary_table {
    uint64_t market;
    symbol_code sell;
    symbol_code pay;
    booksummary supply;
    booksummary demand;
    int deals;
    int blocks;
    uint64_t primary_key() const { return market; }
};

typedef eosio::multi_index< "ordersummary"_n, ordersummary_table > ordersummary;
