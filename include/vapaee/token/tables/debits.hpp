// -- debits --
// This table contains all the permissions pre-assigned to third parties so that they can make debits against the user's balance.

// scope: owner
// row: a fonfiguration for one collector


TABLE debits_table {
    name collector;                // debt collector
    asset max_total;               // max total amount for debits. Each debit decrements this number.
    asset max_per_debit;           // max amount per debt (net max)
    double max_percent;            // max percentage per debt over the current balance
    time_point_sec expiration;     // expiration date
    uint64_t primary_key()const { return collector.value; }
};

typedef eosio::multi_index<"debits"_n, debits_table> debits;
