// --- ballots ---
// This DEX has DAO capabilities using Telos Works. If any user initiates a ballot,
// the Telos Works system is called to create a ballot with the id ballot_name

// scope: contract
// row: represent a ballot taking place in the DEX DAO system

TABLE ballots_table {
    uint64_t id;
    name ballot_name;
    name operation;
    vector<string> params;
    name feepayer;
    time_point_sec started;
    time_point_sec finished;
    string arguments; // answer to why does this ballot even exist
    bool approved;
    bool accepted;
    map<name, asset> results;
    uint64_t primary_key() const { return ballot_name.value; }
    uint64_t by_ballotname_key() const { return ballot_name.value; }
    uint64_t by_operation_key() const { return operation.value; }
    uint64_t by_finished_key() const { return (uint64_t) finished.sec_since_epoch(); }
};

typedef eosio::multi_index< "ballots"_n, ballots_table,
    indexed_by<"ballotname"_n, const_mem_fun<ballots_table, uint64_t, &ballots_table::by_ballotname_key>>,
    indexed_by<"operation"_n, const_mem_fun<ballots_table, uint64_t, &ballots_table::by_operation_key>>,
    indexed_by<"finished"_n, const_mem_fun<ballots_table, uint64_t, &ballots_table::by_finished_key>>
> ballots;
