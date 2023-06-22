
#define TARGET_NOT_VALID     -1
#define TARGET_UNSET          0
#define TARGET_ACCOUNT        1
#define TARGET_PAYHUB         2
#define TARGET_POOL           3
#define TARGET_NAME           4
#define TARGET_PAYHUB_ACCOUNT 5

#include "../datatypes/pool_id.hpp"

struct payhub_target {
    string alias;    // It can refer to one of these options:
                      // -1 - TARGET_NOT_VALID not valid
                      //  0 - TARGET_UNSET no specified yet
                      //  1 - TARGET_ACCOUNT it is a name and exists the account -> Telos account
                      //  2 - TARGET_PAYHUB it is a number -> PayHub(id)
                      //  2 - TARGET_POOL it is a "<symbol_code>-<name>" -> REX pool (token & poollabel)
                      //  4 - TARGET_NAME it is a simple string -> must exist a (vip) name -> PayHub
                      //  5 - TARGET_PAYHUB_ACCOUNT it is an account in a payhub 
    int type;         // target type.
    
    name account;     // type == TARGET_ACCOUNT or TARGET_PAYHUB_ACCOUNT
    uint64_t payhub;  // type == TARGET_PAYHUB or type == TARGET_NAME
    vapaee::pay::pool_id pool;     // type == TARGET_POOL

    std::string to_string() const {
        switch (type) {
            case TARGET_PAYHUB:
                return string("target:") + alias + " type:payhub id:" + std::to_string((unsigned long long)payhub);
            case TARGET_POOL:
                return string("target:") + alias + " type:pool id:" + pool.to_string();
            case TARGET_NAME:
                return string("target:") + alias + " type:name account:" + ((account.value > 0) ? (account.to_string() + " ") : "") +"id: " + std::to_string((unsigned long long)payhub);
            case TARGET_ACCOUNT:
                return string("target:") + alias + " type:account account:" + account.to_string();
            case TARGET_PAYHUB_ACCOUNT:
                return string("target:") + alias + " type:node_account account:" + account.to_string();
            default:
                return string("target:") + alias + " type:unknown";
        }
    };
};
