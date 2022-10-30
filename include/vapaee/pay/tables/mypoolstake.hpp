// -- mypoolstake --
// Each user can stake tokens in different pools, for each pool this table holds the staking state and participation of the user.

// scope: owner
// row: state of the user staking for this pool
struct maturing_funds {
    asset stake;                          // staked tokens to be matured
    time_point_sec mature;
};

TABLE mypoolstake_table {
    uint64_t id;                          // auto-incremented (index by symbol_code and poollabel)
    name poollabel;                       // index by poollabel (need symbol)
    asset stake;                          // total staked
    asset rex;                            // total rex
    asset mature;                         // matured tokens abailable for inmediate withdraw
    time_point_sec stake_update;                   // Last time the stake was updated
    std::vector<maturing_funds> maturing; // list of deposited funds that are not matured yet.
    uint64_t primary_key()const { return id; }
    uint128_t by_poolstake() const {
        return vapaee::utils::pack(
            stake.symbol.code().raw(),
            poollabel.value
        );
    }
};

typedef eosio::multi_index<"mypoolstake"_n, mypoolstake_table,
    indexed_by<"poolstake"_n, const_mem_fun<mypoolstake_table, uint128_t, &mypoolstake_table::by_poolstake>>
> mypoolstake;
