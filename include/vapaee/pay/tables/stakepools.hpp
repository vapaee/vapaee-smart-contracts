// -- stakepools --
// Any token can be configured to be staked by others and many pools can be configured for the same token.
// This table holds those pool configurations for 

// scope: token
// row: a pool configuration for the given token

struct pool_id {
    name id;                            // Each pool identifies with the text: "<symcode>-<id>". i.e: "CNT-main", "VPE-canvas", "VPE-dex"
    symbol_code token;                  // Symbol code of the token to stake (equals skope value)
    std::string to_string() const {
        return token.to_string() + "-" + id.to_string();
    };
    uint128_t pack() {
        return vapaee::utils::pack( token.raw(), id.value );
    };    
};

//---------------------------------------
// TODO: find a better place to put this datatype and constants
#define TARGET_ACCOUNT 1
#define TARGET_PAYHUB  2
#define TARGET_POOL    3
#define TARGET_NAME    4

struct payhub_target {
    string target;    // It can refer to one of these options:
                      // -1 - not valid
                      //  0 - no specified yet
                      //  1 - it is a name and exists the account -> Telos account
                      //  2 - it is a number -> PayHub(id)
                      //  2 - it is a "<symbol_code>-<name>" -> REX pool (token & pool_id)
                      //  4 - it is a simple string -> must exist a (vip) name -> PayHub 
    int type;         // target type.
    
    name account;     // type == TARGET_ACCOUNT
    uint64_t payhub;  // type == TARGET_PAYHUB or type == TARGET_NAME
    pool_id pool;     // type == TARGET_POOL
};
//---------------------------------------


TABLE stakepool_table {
    // pool id
    pool_id pool;                       // identifies the pool

    // pool info
    string locktime;                    // Each pool has its own locktime expressed in days as a single integer number (in string format for future multiple uses).
    string title;                       // Readable title
    string desc;                        // Readable description info

    // poll state
    asset pool_stake;                   // Total amount of tokens in staking in this pool
    asset pool_funds;                   // Total amount of real tokens for this pool. pool_funds = pool_stake + droponpool()
    asset pool_rex;                     // Total amount of rex of this pool

    uint64_t primary_key()const { return pool.id.value; }

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
