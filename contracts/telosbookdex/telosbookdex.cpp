#include "telosbookdex.hpp"

#define DEX_ACTIONS (init)(order)(cancel)
#define DEPOSITS_ACTIONS (withdraw)
#define HANDLERS (htransfer)


EOSIO_DISPATCH_DEX (
    vapaee::telosbookdex,
    DEX_ACTIONS DEPOSITS_ACTIONS,
    HANDLERS
)
