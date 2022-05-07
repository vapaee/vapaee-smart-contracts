#include "telosmaindex.hpp"

#define DEX_ACTIONS (newmarket)
#define TOKEN_ACTIONS (addtoken)(updatetoken)(tokenadmin)(settokendata)(addcurrency)(udpcurrency)
#define CLIENT_ACTIONS (addclient)(updateclient)
#define DAO_ACTIONS (balloton)
#define RECORD_ACTIONS (regbookdeal)(regpoolswap)(updpoolswap)
#define GLOBAL_ACTIONS (init)(maintenance)(reward)
#define HANDLERS (hbroadcast)(htransfer)
#define DEBUG_ACTIONS (testdao)(timeoffset)


EOSIO_DISPATCH_DEX (
    vapaee::telosmaindex,
    DEX_ACTIONS TOKEN_ACTIONS CLIENT_ACTIONS DAO_ACTIONS RECORD_ACTIONS GLOBAL_ACTIONS AUX_DEBUG_CODE(DEBUG_ACTIONS),
    HANDLERS
)
