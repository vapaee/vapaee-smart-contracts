#pragma once

// -------- Handler Module ---------

// handle_pay_transfer
#define ERROR_HPT_1 "ERR-HPT-01:  "
#define ERROR_HPT_2 "ERR-HPT-02:  "
#define ERROR_HPT_3 "ERR-HPT-03:  "
#define ERROR_HPT_4 "ERR-HPT-04:  "
#define ERROR_HPT_5 "ERR-HPT-05:  "
#define ERROR_HPT_6 "ERR-HPT-06:  "


// Deposits Module --------------------

// perform_withdraw
#define ERROR_PW_1 "ERR-PW-01:  "
#define ERROR_PW_2 "ERR-PW-02:  "

// aux_withdraw
#define ERROR_AW_1 "ERR-APW-01:  "
#define ERROR_AW_2 "ERR-APW-02:  "

// action_withdraw
#define ERROR_APW_1 "ERR-APW-01: : can't find contract's name for token: "

// aux_substract_deposits
#define ERROR_APSD_1 "ERR-APSD-01: user has no deposits for this symbol to subtract from: "
#define ERROR_APSD_2 "ERR-APSD-02: symbol mismatch: "
#define ERROR_APSD_3 "ERR-APSD-03: user has not enought deposits to subtract from: "

// aux_earn_micro_change
#define ERROR_APEMC_1 "ERR-APEMC-01: user has no deposits for this symbol: "
#define ERROR_APEMC_2 "ERR-APEMC-02: internal and external symbols don't match in code: "

// aux_add_deposits
#define ERROR_APAD_1 "ERR-APAD-01: Inconsistency, owner has signed the transaction but is not the same as ram payer: "
#define ERROR_APAD_2 "ERR-APAD-02: Inconsistency, contract account has signed the transaction but is not the same as ram payer: "
#define ERROR_APAD_3 "ERR-APAD-03: The token is not registered: "
#define ERROR_APAD_4 "ERR-APAD-04: The token is not setted as tradeable. Contact the token's responsible admin: "
#define ERROR_APAD_5 "ERR-APAD-05: symbol mismatch: "

// aux_swapdeposit
#define ERROR_APS_1 "ERR-APS-01: to account does not exist: "
#define ERROR_APS_2 "ERR-APS-02: invalid quantity: "
#define ERROR_APS_3 "ERR-APS-03: must transfer positive quantity: "
#define ERROR_APS_4 "ERR-APS-04: memo has more than 256 bytes: "

