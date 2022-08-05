#include "telospooldex.hpp"

#define FUNDING_ACTIONS (init)(cancelfund)(takepart)(selftransf)
#define HANDLERS (htransfer)


EOSIO_DISPATCH_DEX (
    vapaee::telospooldex,
    FUNDING_ACTIONS,
    HANDLERS
)
