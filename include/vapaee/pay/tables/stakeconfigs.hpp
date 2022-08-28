// -- stakeconfigs --
// Any token can be configured to be staked by others. This configuration consists of a single global entry and one entry for each staking pool the token may have.
// This table holds the global configuration for each token that can be staked.
// 

// scope: contract
// row: represents a staking global configuration for a token
struct history_entry;

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
    vector<history_entry> history; // recent history of the pool
    
    uint64_t primary_key()const { return token.raw(); }
};

typedef eosio::multi_index<"stakeconfigs"_n, stakeconfigs_table> stakeconfigs;
