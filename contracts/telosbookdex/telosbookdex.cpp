#include "telosbookdex.hpp"

#define DEX_ACTIONS (order)(cancel)
#define TOKEN_ACTIONS (addtoken)(updatetoken)(tokenadmin)(setcurrency)(settokendata)(edittkevent)(addtnkgroup)(uptnkgroup)(chnggroups)
#define CLIENT_ACTIONS (addclient)(updateclient)
#define DAO_ACTIONS (balloton)
#define DEPOSITS_ACTIONS (withdraw)(swapdeposit)(deps2earn)
#define GLOBAL_ACTIONS (init)(maintenance)(reward)
#define HANDLERS (htransfer)(hbroadcast)
#define DEBUG_ACTIONS (hotfix)(testdao)(timeoffset)


EOSIO_DISPATCH_DEX (
    vapaee::telosbookdex,
    DEX_ACTIONS TOKEN_ACTIONS CLIENT_ACTIONS DAO_ACTIONS DEPOSITS_ACTIONS GLOBAL_ACTIONS AUX_DEBUG_CODE(DEBUG_ACTIONS),
    HANDLERS
)
