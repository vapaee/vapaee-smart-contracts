// -- poolstaking --
// Each user can stake tokens in different pools, for each pool this table holds the staking state and participation of the user.

// scope: owner
// row: state of the user staking for this pool
struct maturing_funds {
    asset stake;                          // staked tokens to be matured
    time_point_sec mature;
};

TABLE poolstaking_table {
    uint64_t id;                          // auto-incremented (index by symbol_code and pool_id)
    name pool_id;                         // index by pool_id (need symbol)
    asset stake;                          // total staked
    asset rex;                            // total rex
    asset mature;                         // matured tokens abailable for inmediate withdraw
    std::vector<maturing_funds> maturing; // list of deposited funds that are not matured yet.
    uint64_t primary_key()const { return id; }
    uint128_t by_poolstake() const {
        return vapaee::utils::pack(
            stake.symbol.code().raw(),
            pool_id.value
        );
    }
};

typedef eosio::multi_index<"poolstaking"_n, poolstaking_table,
    indexed_by<"poolstake"_n, const_mem_fun<poolstaking_table, uint128_t, &poolstaking_table::by_poolstake>>
> poolstaking;
