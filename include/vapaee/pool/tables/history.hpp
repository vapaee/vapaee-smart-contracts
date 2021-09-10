struct [[eosio::table]] history_table {
    uint64_t auto_id;
    uint64_t pool_id;
    time_point date;
    name sender;
    name recipient;
    string memo;

    asset sent;
    asset result;

    uint64_t primary_key() const { return auto_id; }
    uint64_t by_pool_key() const { return pool_id; }
    uint64_t by_date_key() const { return (uint64_t) date.sec_since_epoch(); }
    uint64_t by_sender_key() const { return sender.value; }
    uint64_t by_recipient_key() const { return recipient.value; }
};

// scope: contract 
typedef eosio::multi_index<"history"_n, history_table,
    indexed_by<"pool"_n, const_mem_fun<history_table, uint64_t, &history_table::by_pool_key>>,
    indexed_by<"date"_n, const_mem_fun<history_table, uint64_t, &history_table::by_date_key>>,
    indexed_by<"sender"_n, const_mem_fun<history_table, uint64_t, &history_table::by_sender_key>>,
    indexed_by<"recipient"_n, const_mem_fun<history_table, uint64_t, &history_table::by_recipient_key>>
> history;

