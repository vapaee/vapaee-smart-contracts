#include "telosprofile.hpp"

#define CORE_ACTIONS (test)
#define HANDLERS (htransfer)
#define DEBUG_ACTIONS (hotfix)


EOSIO_DISPATCH_DEX (
    vapaee::telosprofile,
    CORE_ACTIONS AUX_DEBUG_CODE(DEBUG_ACTIONS),
    HANDLERS
)
