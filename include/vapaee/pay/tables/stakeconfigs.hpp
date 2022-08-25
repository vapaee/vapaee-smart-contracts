// -- stakeconfigs --
// Any token can be configured to be staked by others. This configuration consists of a single global entry and one entry for each staking pool the token may have.
// This table holds the global configuration for each token that can be staked.
// 

// scope: contract
// row: represents a staking global configuration for a token
struct stakeconfigs_snapshot {
    time_point_sec date;                   // date of the snapshot
    asset total_stake;                     // Total amount of tokens in staking in all pools together
    asset total_funds;                     // Total amount of real tokens backing all stakings pools together

    std::string to_string() const {
        return string("stakeconfigs [date:") + std::to_string((unsigned long)date.utc_seconds) + " total_stake:" + total_stake.to_string() + " total_funds:" + total_funds.to_string() + "]";
    };
};

TABLE stakeconfigs_table {
    symbol_code token;                     // Symbol code of the token to stake (key). It must be registered in telosmaindex
    name admin;                            // account responsible for this staking config

    // stake info
    string title;                          // Readable title

    // and config
    std::vector<name> categories;          // list of options in which the stakers can distribute their staking credits
    string credits_locktime;               // user may cange their credits' categories after a locktime period.

    // global state
    asset total_stake;                     // Total amount of tokens in staking in all pools together
    asset total_funds;                     // Total amount of real tokens backing all stakings pools together

    // vector of recent snapshots
    vector<stakeconfigs_snapshot> history; // recent history of the pool
    
    uint64_t primary_key()const { return token.raw(); }
};

typedef eosio::multi_index<"stakeconfigs"_n, stakeconfigs_table> stakeconfigs;
