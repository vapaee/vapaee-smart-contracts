#include "teloswrapper.hpp"

#define MAINTENANCE_ACTIONS (maintenance)
#define INVEST_ACTIONS (profits)
#define LOANS_ACTIONS (initloan)(cancelloan)(redeem)
#define VOTER_ACTIONS (init)
#define HANDLERS (htransfer)


EOSIO_DISPATCH_DEX (
    vapaee::teloswrapper,
    MAINTENANCE_ACTIONS INVEST_ACTIONS LOANS_ACTIONS VOTER_ACTIONS,
    HANDLERS
)
