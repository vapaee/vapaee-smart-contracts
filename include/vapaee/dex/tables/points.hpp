// --  points (temporal points of the user) -----------
// This DEX rewards users for their actions. Not only gives permanent experience (EXP)
// but also temporal points (POINTS). This table store those temporal points in blocks for a week of time.

// scope: contract
// row: represents a points score ranking for a given week

TABLE points_table {
    uint64_t id;
    uint32_t week;
    name owner;      
    asset points;
    uint64_t primary_key() const { return id; }
    uint64_t by_points_key() const { return points.amount; }
    uint64_t by_owner_key() const { return owner.value; }
};

typedef eosio::multi_index< "points"_n, points_table,
    indexed_by<"points"_n, const_mem_fun<points_table, uint64_t, &points_table::by_points_key>>,
    indexed_by<"owner"_n, const_mem_fun<points_table, uint64_t, &points_table::by_owner_key>>
> points;
