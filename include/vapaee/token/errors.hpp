#pragma once

// -------- Standard Module ---------
// action_transfer
#define ERROR_AT_1 "ERR-AT-1: can not transfer tokens to self "
#define ERROR_AT_2 "ERR-AT-2: 'to' account does not exist "
#define ERROR_AT_3 "ERR-AT-3: The symbol code is not registered in stat table. Verify you are calling the corret token contract for symbol "

// sub_balance
#define ERROR_SB_1 "ERR-SB-1: no balance object found for asset symbol code "
#define ERROR_SB_2 "ERR-SB-2: overdrawn balance [account, balance, requested] "

// action_open
#define ERROR_AO_1 "ERR-AO-1: owner account does not exist "
#define ERROR_AO_2 "ERR-AO-2: symbol does not exist in stat table "
#define ERROR_AO_3 "ERR-AO-3: symbol precision mismatch "

// action_retire
#define ERROR_AR_1 "ERR-AR-1: invalid symbol name "
#define ERROR_AR_2 "ERR-AR-2: memo has more than 256 bytes "
#define ERROR_AR_3 "ERR-AR-3: token with symbol does not exist in table stat "
#define ERROR_AR_4 "ERR-AR-4: invalid quantity "
#define ERROR_AR_5 "ERR-AR-5: must retire positive quantity "
#define ERROR_AR_6 "ERR-AR-6: symbol precision mismatch "

// action_issue
#define ERROR_AI_1 "ERR-AI-1: "
#define ERROR_AI_2 "ERR-AI-2: "
#define ERROR_AI_3 "ERR-AI-3: "
#define ERROR_AI_4 "ERR-AI-4: "
#define ERROR_AI_5 "ERR-AI-5: "
#define ERROR_AI_6 "ERR-AI-6: "
#define ERROR_AI_7 "ERR-AI-7: symbol precision mismatch (basictoken) "




// -------- Handler Module ---------

// handle_token_transfer
#define ERROR_HTT_1 "ERR-HTT-1"
#define ERROR_HTT_2 "ERR-HTT-2"
#define ERROR_HTT_3 "ERR-HTT-3"
#define ERROR_HTT_4 "ERR-HTT-4"
#define ERROR_HTT_5 "ERR-HTT-5"
#define ERROR_HTT_6 "ERR-HTT-6"
#define ERROR_HTT_7 "ERR-HTT-7"

