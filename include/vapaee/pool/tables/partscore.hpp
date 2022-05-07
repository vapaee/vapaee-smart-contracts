// --- partscore ---
// Participation Score is the percentage of the total amount of tokens that a player has in the pool.

// scope: pool id
// row: a participation score for a funder

TABLE part_score_table {
    name funder;
    asset score;

    uint64_t primary_key() const { return funder.value; }
    uint64_t by_score() const { return score.amount; }
};

typedef eosio::multi_index<"partscore"_n, part_score_table,
    indexed_by<"score"_n, const_mem_fun<part_score_table, uint64_t, &part_score_table::by_score>>
> partscore;

