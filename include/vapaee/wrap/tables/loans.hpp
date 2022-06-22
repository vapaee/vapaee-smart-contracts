// -- loans --
// Each time a user lend a new entry is created in this table. one user can have several independent loans opened at the same time.

// scope: contract
// row: a loan taken by a user
TABLE loan_table {
    uint64_t id;                  // loan taked from global::current_shot
    name owner;                   // user who requested the loan
    asset debt;
    asset rex;
    time_point_sec created_at;    // date of creation. Useful to know if the user can cancel the loan
    // time_point_sec updated_at;    // date of last update
    // time_point_sec redeemed_at;   // date of last redeemed
    // uint64_t last_shot;           // last rexshot proccessed to get calculate profits 
    // asset profits;                // accumulated profits in TLOS

    uint64_t primary_key() const { return id; }
    uint64_t by_owner() const { return owner.value; }
    // uint64_t by_debt_tlosv() const { return debt.tlosv.amount; }
    // uint64_t by_debt_tlosw() const { return debt.tlosw.amount; }
    // uint64_t by_lastshot() const { return last_shot; }
    // uint64_t by_updated() const { return updated_at.sec_since_epoch(); }
    // uint64_t by_created() const { return created_at.sec_since_epoch(); }
    // uint64_t by_redeemed() const { return redeemed_at.sec_since_epoch(); }
    // uint64_t by_canceling() const { return canceling.total().amount; }
};

typedef eosio::multi_index<"loans"_n, loan_table,
    indexed_by<"owner"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_owner>>
    // indexed_by<"debttlosv"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_debt_tlosv>>,
    // indexed_by<"debttlosw"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_debt_tlosw>>,
    // exed_by<"lastshot"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_lastshot>>,
    // exed_by<"updated"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_updated>>,
    // indexed_by<"created"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_created>>,
    // exed_by<"redeemed"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_redeemed>>,
    // exed_by<"canceling"_n, const_mem_fun<loan_table, uint64_t, &loan_table::by_canceling>>
> loans;
