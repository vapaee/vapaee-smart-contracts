#pragma once

// Handler Module ----------------------

// handle_pool_transfer
#define ERROR_HPT_0 "ERR-HPT-00: Swap fee is not set in FEE symbol. Is it even initialized? "
#define ERROR_HPT_1 "ERR-HPT-01: Token not valid. "
#define ERROR_HPT_2 "ERR-HPT-02: The memo must contain process information separated by commas. "
#define ERROR_HPT_3 "ERR-HPT-03: Couln't extract the pool id. When funding, the memo must contain exactly two pieces of information separated by commas (eg: 'fund,CNT/TLOS'). "
#define ERROR_HPT_4 "ERR-HPT-04: Memo is not valid. swapfund need to indicate the pool and optionally the owner (if is different from the sender). "
#define ERROR_HPT_5 "ERR-HPT-05: Couln't extract the pool id. When sending profits, the memo must contain exactly two pieces of information separated by commas (eg: 'fund,CNT/TLOS'). "
#define ERROR_HPT_6 "ERR-HPT-06: When swapping, the memo must contain exactly five information separated by commas (eg: 'openpool.v1,telospooldex/TLOS telospooldex/CNT,1.1234 CNT,alice,optional memo'). "
#define ERROR_HPT_7 "ERR-HPT-07: Memo is not valid. (valid examples are: 'profits', 'swapfund,CNT/DEX,alice', 'fund,CNT/DEX' or 'openpool.v1,telospooldex/DEX telospooldex/CNT,1.1234 CNT,alice,optional memo'). got: "


// Liquidity Module ----------------------

// action_cancel_fund
#define ERROR_ACF_1 "ERR-ACF-01: Fund attempt not found. "
#define ERROR_ACF_2 "ERR-ACF-02: Market not found. "

// fund_attempt
#define ERROR_FA_1 "ERR-FA-01: We need a canonical market id needed but got inverse market id. "
#define ERROR_FA_2 "ERR-FA-02: Market not not found for given id. "
#define ERROR_FA_3 "ERR-FA-03: The second market symbol is not the pool currency. "
#define ERROR_FA_4 "ERR-FA-04: The first market symbol is not the pool currency. "
#define ERROR_FA_5 "ERR-FA-05: None of the market symbols belong to this pool. "
#define ERROR_FA_6 "ERR-FA-06: Fund attempt not found and could not be created. "

// add_profits
#define ERROR_AP_1 "ERR-AP-01: Market not not found for given id. "
#define ERROR_AP_2 "ERR-AP-02: Quantity symbol does not match with any of the pool's token reserves. "



// SWAP Module ----------------------

// convert
#define ERROR_C_1 "ERR-C-01: Recipient account does not exist. "
#define ERROR_C_2 "ERR-C-02: First step on path is not well formed. Needed: 'tokenaccount/SYMBOL' but got "
#define ERROR_C_3 "ERR-C-03: First step on path must be the same as converter (step, converter):"
#define ERROR_C_4 "ERR-C-04: Pool does not have any commodity funds. "
#define ERROR_C_5 "ERR-C-05: Pool does not have any currency funds. "
#define ERROR_C_6 "ERR-C-06: Next step on path is not well formed. Needed: 'tokenaccount/SYMBOL' but got "
#define ERROR_C_7 "ERR-C-07: Converter not found. "
#define ERROR_C_8 "ERR-C-08: Minimun not reached, so the swap is canceled."
#define ERROR_C_9 "ERR-C-09: pool not found for tokens (tokenA, tokenB, contract): "

// migratted from pool::utils
#define ARITHMETIC_PRECISION 8

#define ERR_MARKET_NOT_FOUND    "market not found"
#define ERR_MARKET_INVERSE      "market musn\'t be inverted"
#define ERR_POOL_EXISTS         "pool already exists"
#define ERR_TOKEN_NOT_REG       "token not registered"
#define ERR_COMM_SYM_NOT_MATCH  "commodity symbols don\'t match"
#define ERR_CURR_SYM_NOT_MATCH  "currency symbols don\'t match"
#define ERR_ACCOUNT_NOT_FOUND   "account not found"
#define ERR_MEMO_PARSING        "incorrect memo format"
#define ERR_ATTEMPT_NOT_FOUND   "fund attempt not found"
#define ERR_NOT_FUNDER          "not funder"
#define ERR_EMPTY_PATH          "path is empty"
#define ERR_INCORRECT_CONVERTER "this is not the converter you need"
#define ERR_POOL_NOT_FUNDED     "unfunded pool"
#define ERR_RECIPIENT_NOT_FOUND "recipient not found"
#define ERR_BAD_DEAL            "total less than minimun"
#define ERR_CONVERTER_NOT_FOUND "can't find converter"
#define ERR_FAKE_TOKEN          "wrong token contract"
#define ERR_INSUFFICENT_PART    "insufficent participation"
#define THANK_YOU_MSG "Thank you for using Telos Pool DEX"


// UTILS Module ----------------------

// create_fund_attempt
#define ERROR_CFA_1 "ERR-CFA-01: pool not found for market id (id, contract): "
