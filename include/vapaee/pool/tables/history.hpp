struct [[eosio::table]] history_table {
    uint64_t id;
    time_point_sec date;
    name buyer;
    name seller;
    asset price;
    asset inverse;
    asset amount;
    asset payment;
    asset buyfee;
    asset sellfee;
    bool isbuy;
    uint64_t primary_key() const { return id; }
    uint64_t by_date_key() const { return (uint64_t) date.sec_since_epoch(); }
    uint64_t by_buyer_key() const { return buyer.value; }
    uint64_t by_seller_key() const { return seller.value; }
    uint64_t by_payment_key() const { return payment.amount; }
};

// scope: market-id
typedef eosio::multi_index<"convhistory"_n, history_table,
    indexed_by<"date"_n, const_mem_fun<history_table, uint64_t, &history_table::by_date_key>>,
    indexed_by<"buyer"_n, const_mem_fun<history_table, uint64_t, &history_table::by_buyer_key>>,
    indexed_by<"seller"_n, const_mem_fun<history_table, uint64_t, &history_table::by_seller_key>>,
    indexed_by<"payment"_n, const_mem_fun<history_table, uint64_t, &history_table::by_payment_key>>
> conv_history;


// scope: market-id
typedef eosio::multi_index<"fundhistory"_n, history_table,
    indexed_by<"date"_n, const_mem_fun<history_table, uint64_t, &history_table::by_date_key>>,
    indexed_by<"buyer"_n, const_mem_fun<history_table, uint64_t, &history_table::by_buyer_key>>,
    indexed_by<"seller"_n, const_mem_fun<history_table, uint64_t, &history_table::by_seller_key>>,
    indexed_by<"payment"_n, const_mem_fun<history_table, uint64_t, &history_table::by_payment_key>>
> fund_history;

