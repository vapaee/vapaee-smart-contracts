#include "telosmaindex.hpp"

#define DEX_ACTIONS (newmarket)
#define TOKEN_ACTIONS (addtoken)(updatetoken)(tokenadmin)(setcurrency)(settokendata)(addtnkgroup)(uptnkgroup)(chnggroups)
#define CLIENT_ACTIONS (addclient)(updateclient)
#define DAO_ACTIONS (balloton)
#define GLOBAL_ACTIONS (init)(maintenance)(reward)
#define HANDLERS (hbroadcast)
#define DEBUG_ACTIONS (testdao)(timeoffset)


EOSIO_DISPATCH_DEX (
    vapaee::telosmaindex,
    DEX_ACTIONS TOKEN_ACTIONS CLIENT_ACTIONS DAO_ACTIONS GLOBAL_ACTIONS AUX_DEBUG_CODE(DEBUG_ACTIONS),
    HANDLERS
)
