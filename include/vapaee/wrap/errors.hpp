#pragma once

// -------- Handler Module ---------

// handle_wrap_transfer
#define ERROR_HWT_1 "ERR-HWT-01:  "
#define ERROR_HWT_2 "ERR-HWT-02:  "
#define ERROR_HWT_3 "ERR-HWT-03:  "
#define ERROR_HWT_4 "ERR-HWT-04:  "
#define ERROR_HWT_5 "ERR-HWT-05:  "
#define ERROR_HWT_6 "ERR-HWT-06:  "
#define ERROR_HWT_7 "ERR-HWT-07:  "


// check_loan_memo
#define ERROR_CLM_1 "ERR-CLM-01:  "
#define ERROR_CLM_2 "ERR-CLM-02:  "
#define ERROR_CLM_3 "ERR-CLM-03:  "
#define ERROR_CLM_4 "ERR-CLM-04:  "
#define ERROR_CLM_5 "ERR-CLM-05:  "

// check_profits_memo
#define ERROR_CPM_1 "ERR-CPM-01:  "

// check_cancel_or_changedebt_memo
#define ERROR_CCOCM_1 "ERR-CCOCM-01:  "
#define ERROR_CCOCM_2 "ERR-CCOCM-02:  "


// handle_possible_swap_conversion
#define ERROR_HPCS_1 "ERR-HPCS-01:  "

// -------- Maintenance Module ---------

// aux_maintenance_for_specific_loan
#define ERROR_AMFSL_1 "ERR-AMFSL-01:  "
#define ERROR_AMFSL_2 "ERR-AMFSL-02:  "

// do_schedule_withdraw
#define ERROR_DSW_1 "ERR-DSW-01:  "
#define ERROR_DSW_2 "ERR-DSW-02:  "
#define ERROR_DSW_3 "ERR-DSW-03:  "

// get_maximum_rex_to_sell_from_loan
#define ERROR_GMRTSFL_1 "ERR-GMRTSFL-01:  "
#define ERROR_GMRTSFL_2 "ERR-GMRTSFL-02:  "
#define ERROR_GMRTSFL_3 "ERR-GMRTSFL-03:  "



// -------- SWAP Module -----------

// convert
#define ERROR_SCV_1 "ERR-SCV-01:  "
#define ERROR_SCV_2 "ERR-SCV-02:  "
#define ERROR_SCV_3 "ERR-SCV-03:  "
#define ERROR_SCV_4 "ERR-SCV-04:  "
#define ERROR_SCV_5 "ERR-SCV-05:  "
#define ERROR_SCV_6 "ERR-SCV-06:  "
#define ERROR_SCV_7 "ERR-SCV-07:  "
#define ERROR_SCV_8 "ERR-SCV-08:  "




// -------- Lending Module -----------

// handle_loan
#define ERROR_HL_1 "ERR-HL-01:  "
#define ERROR_HL_2 "ERR-HL-02:  "
#define ERROR_HL_3 "ERR-HL-03:  "
#define ERROR_HL_4 "ERR-HL-04:  "

// handle_cancel_user_loan
#define ERROR_HCUL_1 "ERR-HCUL-01:  "
#define ERROR_HCUL_2 "ERR-HCUL-02:  "
#define ERROR_HCUL_3 "ERR-HCUL-03:  "
#define ERROR_HCUL_4 "ERR-HCUL-04:  "
#define ERROR_HCUL_5 "ERR-HCUL-05:  "


// handle_redeem
#define ERROR_HR_1 "ERR-HR-01:  "

// handle_change_debt
#define ERROR_HCD_1 "ERR-HCD-01:  "
#define ERROR_HCD_2 "ERR-HCD-02:  "
#define ERROR_HCD_3 "ERR-HCD-03:  "
#define ERROR_HCD_4 "ERR-HCD-04:  "

// add_to_loan_canceling
#define ERROR_ATLC_1 "ERR-ATLC-01:  "

// get_empty_loan_id_for_user
#define ERROR_GELIUFU_1 "ERR-GELIUFU-01: User must call action initloan before taken a real loan. Couldn't get empty loan id for user "

// action_cancel_loan
#define ERROR_ACL_1 "ERR-ACL-01:  "
#define ERROR_ACL_2 "ERR-ACL-02:  "
#define ERROR_ACL_3 "ERR-ACL-03:  "
#define ERROR_ACL_4 "ERR-ACL-04:  "
#define ERROR_ACL_5 "ERR-ACL-05:  "
#define ERROR_ACL_6 "ERR-ACL-06:  "
#define ERROR_ACL_7 "ERR-ACL-07:  "


// delegate_oldest_loan
#define ERROR_AOL_1 "ERR-AOL-01:  "
#define ERROR_AOL_2 "ERR-AOL-02:  "


// -------- Investments Module ----------

// handle_telos_profits
#define ERROR_HTP_1 "ERR-HTP-01:  "
#define ERROR_HTP_2 "ERR-HTP-02:  "
#define ERROR_HTP_3 "ERR-HTP-03:  "

// handle_tlosv_profits
#define ERROR_HTVP_1 "ERR-HTVP-04:  "
#define ERROR_HTVP_2 "ERR-HTVP-05:  "
#define ERROR_HTVP_3 "ERR-HTVP-06:  "

// handle_telosw_profits
#define ERROR_HTWP_1 "ERR-HTWP-01:  "
#define ERROR_HTWP_2 "ERR-HTWP-02:  "
#define ERROR_HTWP_3 "ERR-HTWP-03:  "

// handle_custom_token_profits
#define ERROR_HCTP_1 "ERR-HCTP-01:  "
#define ERROR_HCTP_2 "ERR-HCTP-02:  "
#define ERROR_HCTP_3 "ERR-HCTP-03:  "

// handle_profits
#define ERROR_HP_1 "ERR-HP-01:  "
#define ERROR_HP_2 "ERR-HP-02:  "
#define ERROR_HP_3 "ERR-HP-03:  "

// spend_some_tlosv_to_delegate_oldest_loan
#define ERROR_SSTAVOL_1 "ERR-SSTAVOL-01:  "
#define ERROR_SSTAVOL_2 "ERR-SSTAVOL-02:  "
#define ERROR_SSTAVOL_3 "ERR-SSTAVOL-03:  "

// send_telosv_as_pool_profits
#define ERROR_STAPP_1 "ERR-STAPP-01:  "
#define ERROR_STAPP_2 "ERR-STAPP-02:  "
#define ERROR_STAPP_3 "ERR-STAPP-03:  "

// send_telosv_to_swapfund_pools
#define ERROR_STTSP_1 "ERR-STTSP-01:  "
#define ERROR_STTSP_2 "ERR-STTSP-02:  "
#define ERROR_STTSP_3 "ERR-STTSP-03:  "


// -------- Global Module ----------

// // action_init
// #define ERROR_AI_1 "ERR-AI-01:  "
// #define ERROR_AI_2 "ERR-AI-02:  "

// get_global_state
#define ERROR_GGS_1 "ERR-GGS-01:  "

// action_updatestate
// #define ERROR_AUS_1 "ERR-AUS-01:  "

// add_loan
#define ERROR_AL_1 "ERR-AL-01:  "

// update_snapshot
#define ERROR_US_1 "ERR-US-01:  "
#define ERROR_US_2 "ERR-US-02:  "
#define ERROR_US_3 "ERR-US-03:  "

// get_snapshot
#define ERROR_GS_1 "ERR-GS-01:  "

// add_liquidity
#define ERROR_ALI_1 "ERR-ALI-01:  "
#define ERROR_ALI_2 "ERR-ALI-02:  "

// remove_liquidity
#define ERROR_RLI_1 "ERR-RLI-01:  "
#define ERROR_RLI_2 "ERR-RLI-02:  "
#define ERROR_RLI_3 "ERR-RLI-03:  "


// REX Module ----------------------

// get_rex_balance
#define ERROR_GRB_1 "ERR-GRB-01:  "
#define ERROR_GRB_2 "ERR-GRB-02:  "
#define ERROR_GRB_3 "ERR-GRB-03:  "

// get_rex_pool
#define ERROR_GRP_1 "ERR-GRP-01:  "
#define ERROR_GRP_2 "ERR-GRP-02:  "
#define ERROR_GRP_3 "ERR-GRP-03:  "

// get_rex_fund
#define ERROR_GRF_1 "ERR-GRF-01:  "

// deposit_tlos_to_rex
#define ERROR_DTTR_1 "ERR-DTTR-01:  "

// withdraw_REX
#define ERROR_WDR_1 "ERR-WDR-01:  "

// withdraw_TLOS
#define ERROR_WDT_1 "ERR-WDT-01:  "

