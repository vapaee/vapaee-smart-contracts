// -- stakepools --
// Any token can be configured to be staked by others and many pools can be configured for the same token.
// This table holds those pool configurations for 

// scope: token
// row: a pool configuration for the given token

// #include "../datatypes/pool_id.hpp"
#include "../datatypes/history_entry.hpp"

TABLE stakepool_table {
    // pool id
    pool_id id;                         // identifies the pool

    // pool info
    string locktime;                    // Each pool has its own locktime expressed in days as a single integer number (in string format for future multiple uses).
    string title;                       // Readable title

    // poll state
    asset pool_stake;                   // Total amount of tokens in staking in this pool
    asset pool_funds;                   // Total amount of real tokens for this pool. pool_funds = pool_stake + droponpool()
    asset pool_rex;                     // Total amount of rex of this pool

    // vector of recent snapshots
    vector<history_entry> history;      // recent history of the pool

    uint64_t primary_key()const { return id.label.value; }

    double rex_price() const {
        if (pool_rex.amount == 0) {
            return 1.0;
        } else {
            return (double) pool_funds.amount / (double) pool_rex.amount;
        }
    }

    double rex_inverse() const {
        return 1.0 / rex_price();
    }
};

typedef eosio::multi_index<"stakepools"_n, stakepool_table> stakepools;
