// -- mypoolstake --
// Each user can stake tokens in different pools, for each pool this table
// holds the staking state and participation of the user.
// Each mypoolstake has a list or mature slots that holds a qantity and a mature date.
// The user initialy starts with ony one slot but more slots can be bought.
// If more tokens are staked and the user has no empty slots availables,
// then add quantity to the last updated slot and reset the mature date

// scope: owner
// row: state of the user staking for this pool
struct maturing_slot {
    asset stake;                          // staked tokens to be matured
    time_point_sec mature;

    std::string to_string() const {
        return string("slot [mature:") + std::to_string((unsigned long)mature.utc_seconds) + " stake:" + stake.to_string() + "]";
    };
};

TABLE mypoolstake_table {
    uint64_t id;                          // auto-incremented (index by symbol_code and poollabel)
    name poollabel;                       // index by poollabel (need symbol)
    asset stake;                          // total staked
    asset rex;                            // total rex
    asset mature;                         // matured tokens abailable for inmediate withdraw
    time_point_sec stake_update;          // Last time the stake was updated
    std::vector<maturing_slot> maturing;  // list of deposited funds that are not matured yet.
    int maturing_slots;                   // Indicates the number of slots the user possess  
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
