// -- stakepools --
// Any token can be configured to be staked by others and many pools can be configured for the same token.
// This table holds those pool configurations for 

// scope: token
// row: a pool configuration for the given token

TABLE stakepool_table {
    // pool id
    name id;                            // Each pool identifies with the text: "<symcode>-<id>". i.e: "CNT-main", "VPE-canvas", "VPE-dex"
    symbol_code token;                  // Symbol code of the token to stake (equals skope value)

    // pool info
    string locktime;                    // Each pool has its own locktime expressed in days as a single integer number (in string format for future multiple uses).
    string title;                       // Readable title
    string desc;                        // Readable description info

    // poll state
    asset pool_stake;                   // Total amount of tokens in staking in this pool
    asset pool_funds;                   // Total amount of real tokens for this pool. pool_funds = pool_stake + droponpool()
    asset pool_rex;                     // Total amount of rex of this pool

    uint64_t primary_key()const { return id.value; }

    double rex_price() const {
        if (pool_rex.amount == 0) {
            return 1.0;
        } else {
            return (double) pool_funds.amount / (double) pool_rex.amount;
        }
    }

    double rex_inverse() const {
        if (pool_funds.amount == 0) {
            return 1.0 / rex_price();
        } else {
            return (double) pool_rex.amount / (double) pool_funds.amount;
        }
    }
};

typedef eosio::multi_index<"stakepools"_n, stakepool_table> stakepools;
