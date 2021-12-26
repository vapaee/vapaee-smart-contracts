#pragma once



// Exchange Module ----------------------

// aux_generate_order
 
#define ERROR_AGO_1 "ERR-AGO-01: amount token is blacklisted: "
#define ERROR_AGO_2 "ERR-AGO-02: price token is blacklisted: "

// aux_cancel_sell_order
#define ERROR_ACSO_1 "ERR-ACSO-01: ordersummary table does not exist for that scope: "
#define ERROR_ACSO_2 "ERR-ACSO-02: symbol mismatch: "

// aux_generate_sell_order
#define ERROR_AGSO_1 "ERR-AGSO-01: symbol mismatch: "
#define ERROR_AGSO_2 "ERR-AGSO-02: symbol mismatch: "
#define ERROR_AGSO_3 "ERR-AGSO-03: symbol mismatch: "
#define ERROR_AGSO_4 "ERR-AGSO-04: symbol mismatch: "
#define ERROR_AGSO_5 "ERR-AGSO-05: symbol mismatch: "
#define ERROR_AGSO_6 "ERR-AGSO-06: symbol mismatch: "
#define ERROR_AGSO_7 "ERR-AGSO-07: symbol mismatch: "
#define ERROR_AGSO_8 "ERR-AGSO-08: symbol mismatch: "


// Deposits Module --------------------

// action_withdraw
#define ERROR_AW_1 "ERR-AW-01: : can't find contract's name for token: "

// aux_substract_deposits
#define ERROR_ASD_1 "ERR-ASD-01: user has no deposits for this symbol to subtract from: "
#define ERROR_ASD_2 "ERR-ASD-02: symbol mismatch: "
#define ERROR_ASD_3 "ERR-ASD-03: user has not enought deposits to subtract from: "

// aux_earn_micro_change
#define ERROR_AEMC_1 "ERR-AEMC-01: user has no deposits for this symbol: "
#define ERROR_AEMC_2 "ERR-AEMC-02: internal and external symbols don't match in code: "

// aux_add_deposits
#define ERROR_AAD_1 "ERR-AAD-01: Inconsistency, owner has signed the transaction but is not the same as ram payer: "
#define ERROR_AAD_2 "ERR-AAD-02: Inconsistency, contract account has signed the transaction but is not the same as ram payer: "
#define ERROR_AAD_3 "ERR-AAD-03: The token is not registered: "
#define ERROR_AAD_4 "ERR-AAD-04: The token is not setted as tradeable. Contact the token's responsible admin: "
#define ERROR_AAD_5 "ERR-AAD-05: symbol mismatch: "

// handle_book_transfer
#define ERROR_HBT_1 "ERR-HBT-01: "
