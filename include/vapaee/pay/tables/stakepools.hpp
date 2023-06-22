// -- stakepools --
// Any token can be configured to be staked by others and many pools can be configured for the same token.
// This table holds those pool configurations for 
/*


struct pool_id {
    name label;                         // Each pool identifies with the text: "<symcode>-<label>". i.e: "CNT-main", "VPE-canvas", "VPE-dex"
    symbol_code token;                  // Symbol code of the token to stake (equals skope value)
    std::string to_string() const {
        return token.to_string() + "-" + label.to_string();
    };
    uint128_t pack() {
        return vapaee::utils::pack( token.raw(), label.value );
    };    
};

//---------------------------------------
// TODO: find a better place to put this datatype and constants
#define TARGET_NOT_VALID -1
#define TARGET_UNSET      0
#define TARGET_ACCOUNT    1
#define TARGET_PAYHUB     2
#define TARGET_POOL       3
#define TARGET_NAME       4

struct payhub_target {
    string alias;     // It can refer to one of these options:
                      // -1 - TARGET_NOT_VALID not valid
                      //  0 - TARGET_UNSET no specified yet
                      //  1 - TARGET_ACCOUNT it is a name and exists the account -> Telos account
                      //  2 - TARGET_PAYHUB it is a number -> PayHub(id)
                      //  2 - TARGET_POOL it is a "<symbol_code>-<name>" -> REX pool (token & poollabel)
                      //  4 - TARGET_NAME it is a simple string -> must exist a (vip) name -> PayHub 
    int type;         // target type.
    
    name account;     // type == TARGET_ACCOUNT
    uint64_t payhub;  // type == TARGET_PAYHUB or type == TARGET_NAME
    pool_id pool;     // type == TARGET_POOL

    std::string to_string() const {
        switch (type) {
            case TARGET_PAYHUB:
                return string("alias:") + alias + " type:payhub id:" + std::to_string((unsigned long long)payhub);
            case TARGET_POOL:
                return string("alias:") + alias + " type:pool id:" + pool.to_string();
            case TARGET_NAME:
                return string("alias:") + alias + " type:name account:" + ((account.value > 0) ? (account.to_string() + " ") : "") +"id: " + std::to_string((unsigned long long)payhub);
            case TARGET_ACCOUNT:
                return string("alias:") + alias + " type:account account:" + account.to_string();
            default:
                return string("alias:") + alias + " type:unknown";
        }
    };
};
//---------------------------------------
struct history_entry {
    time_point_sec date;             // date of the snapshot
    asset quantity;                  // 
    std::string to_string() const {
        return string("stakeconfigs [date:") + std::to_string((unsigned long)date.utc_seconds) + " quantity:" + quantity.to_string() + "]";
    };
};
*/
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
