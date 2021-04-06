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

// aux_create_market_and_return_id
#define ERROR_ACMARI_1 "ERR-ACMARI-01: This market is not allowed because none of them are currencies and they are not in the same token group: "

// aux_add_deposits
#define ERROR_AAD_1 "ERR-AAD-01: Inconsistency, owner has signed the transaction but is not the same as ram payer: "
#define ERROR_AAD_2 "ERR-AAD-02: Inconsistency, contract account has signed the transaction but is not the same as ram payer: "
#define ERROR_AAD_3 "ERR-AAD-03: The token is not registered: "
#define ERROR_AAD_4 "ERR-AAD-04: The token is not setted as tradeable. Contact the token's responsible admin: "
#define ERROR_AAD_5 "ERR-AAD-05: symbol mismatch: "


// Market Module ----------------------

// aux_is_A_currency_in_any_B_groups
#define ERROR_AIACIABG_1 "ERR-AIACIABG-01: Can't find tokengroup for token: "

// aux_is_it_allowed_to_cerate_this_market
#define ERROR_AIIATCTM_1 "ERR-AIIATCTM-01: This token is not registered: "
#define ERROR_AIIATCTM_2 "ERR-AIIATCTM-02: This token is not registered: "
#define ERROR_AIIATCTM_3 "ERR-AIIATCTM-03: Can't create market. Both tokens are the same: "

// aux_get_table_from_market
#define ERROR_AGTFM_1 "ERR-AGTFM-01: not market found: "


// Client Module -------------------------

// aux_assert_client_is_valid
#define ERROR_AACIV_1 "ERR-AACIV-01: client not found: "

// action_add_client
#define ERROR_AAC_1 "ERR-AAC-01: receiver account does not exist: "
#define ERROR_AAC_2 "ERR-AAC-02: admin account does not exist: "
#define ERROR_AAC_3 "ERR-AAC-03: admin signature needed: "

// action_update_client
#define ERROR_AUC_1 "ERR-AUC-01: receiver account does not exist: "
#define ERROR_AUC_2 "ERR-AUC-02: admin account does not exist: "
#define ERROR_AUC_3 "ERR-AUC-03: admin signature needed: "

// aux_check_client_exists
#define ERROR_ACCE_1 "ERR-ACCE-01: interface not found with this id: "

// aux_transfer_earnings_to_client
#define ERROR_ATETC_1 "ERR-ATETC-01: interface not found with this id: "

// aux_get_real_asset
#define ERROR_AGEA_1 "ERR-AGEA-01: precision mismatch: "
#define ERROR_AGEA_2 "ERR-AGEA-02: can't find the precision for this symbol because can't fint the token registration: "


// Token Module ----------------------

// action_update_token_info
#define ERROR_AUTI_1 "ERR-AUTI-01: Token not registered. You must register it first calling addtoken action: " 
#define ERROR_AUTI_2 "ERR-AUTI-02: only admin or token's admin can modify the token main info: " 
#define ERROR_AUTI_3 "ERR-AUTI-03: the token is currently blacklisted: " 

// action_change_groups_for_a_token
#define ERROR_ACGFAT_1 "ERR-ACGFAT-01: Token not registered. You must register it first calling addtoken action: " 
#define ERROR_ACGFAT_2 "ERR-ACGFAT-02: only admin or token's admin can modify the token main info: " 
#define ERROR_ACGFAT_3 "ERR-ACGFAT-03: the token is currently blacklisted: " 

// action_add_token
#define ERROR_AAT_1 "ERR-AAT-01: Token with symbol not exists: " 
#define ERROR_AAT_2 "ERR-AAT-02: Token already registered: " 

// action_set_token_admin
#define ERROR_ASTA_1 "ERR-ASTA-01: Token not registered. You must register it first calling addtoken action: " 
#define ERROR_ASTA_2 "ERR-ASTA-02: account does not exist: " 
#define ERROR_ASTA_3 "ERR-ASTA-03: only DAO or token's admin can change token admin" 

// action_add_token_group
#define ERROR_AATG_1 "ERR-AATG-01: admin account does not exist: "
#define ERROR_AATG_2 "ERR-AATG-02: admin signature needed: "

// action_update_token_group
#define ERROR_AUTG_1 "ERR-AUTG-01: admin account does not exist: "
#define ERROR_AUTG_2 "ERR-AUTG-02: admin signature needed: "
#define ERROR_AUTG_3 "ERR-AUTG-03: tokengroup not found with id: "

// action_set_token_as_currency
#define ERROR_ASTAC_1 "ERR-ASTAC-01: Token not registered. You must register it first calling setcurrency action: " 
#define ERROR_ASTAC_2 "ERR-ASTAC-02: Token group not found: " 
#define ERROR_ASTAC_3 "ERR-ASTAC-03: Only contract account can modify the token group zero currency list" 
#define ERROR_ASTAC_4 "ERR-ASTAC-04: Only token group admin can modify currency list: " 
#define ERROR_ASTAC_5 "ERR-ASTAC-05: Only token admin can set this token as currency: " 
#define ERROR_ASTAC_6 "ERR-ASTAC-06: Token is already present in token group as currency: " 
#define ERROR_ASTAC_7 "ERR-ASTAC-07: Token is not present in token group as currency: " 


// Experience Module ------------------------
// action_reward_user
#define ERROR_ARU_1 "ERR-ARU-01: this is a private action and must be signed by contract account"
#define ERROR_ARU_2 "ERR-ARU-02: points asset symbol code does not match with internal representation: "
#define ERROR_ARU_3 "ERR-ARU-03: points asset symbol precision does not match with internal representation: "
#define ERROR_ARU_4 "ERR-ARU-04: exp asset symbol code does not match with internal representation: "
#define ERROR_ARU_5 "ERR-ARU-05: exp asset symbol precision does not match with internal representation: "

// Maintenance Module ------------------------

// aux_maintenance_from_history
#define ERROR_AMFH_1 "ERR-AMFH-01: inconsistancy during maintenance. Trying to prune history (key, scope): "

// DAO Module ------------------------

// action_start_ballot_on
#define ERROR_ASBO_1 "ERR-ASBO-01: argument is too long (1024 chars max). Your's has: "
#define ERROR_ASBO_2 "ERR-ASBO-02: unknown property: "
#define ERROR_ASBO_3 "ERR-ASBO-03: telos.decide fee symbol_code and system symbol_code does not match?: "
#define ERROR_ASBO_4 "ERR-ASBO-04: ballot_name already exists: "

// aux_check_ballot_can_be_created
#define ERROR_ACBABC_3 "ERR-ACBABC-03: already exists an open ballot for this same operation (and params): "

// aux_check_symbol_code_from_string
#define ERROR_ACSCFS_1 "ERR-ACSCFS-01: string is too long to be a valid symbol_code: "
#define ERROR_ACSCFS_2 "ERR-ACSCFS-02: thirteenth character in name cannot be a letter that comes after j: "

// aux_check_name_from_string
#define ERROR_ACNFS_1 "ERR-ACNFS-01: contract name is too long to be a valid name: "
#define ERROR_ACNFS_2 "ERR-ACNFS-02: contract name can not be empty: "
#define ERROR_ACNFS_3 "ERR-ACNFS-03: thirteenth character of contract name cannot be a letter that comes after j: "

// char_to_value
#define ERROR_CTV_1 "ERR-CTV-1: character is not in allowed character set for names: "

// get (config)
#define ERROR_GC_1 "ERR-GC-1: table not initialized: " 

// action_init_config
#define ERROR_AIC_1 "ERR-AIC-1: config already initialized! " 

// aux_check_double_from_string
#define ERROR_ACDFS_1 "ERR-ACDFS-01: Fee can not be negative: "
#define ERROR_ACDFS_2 "ERR-ACDFS-02: Fee can not higher than 1: "

// aux_check_integer_from_string
#define ERROR_ACIFS_1 "ERR-ACIFS-02: Cant prune future records: "

// aux_get_telos_decide_config
#define ERROR_AGTDC_1 "ERR-AGTDC-01: Can't find Telos Decide config in contract: "

// aux_get_telos_decide_treasure_supply
#define ERROR_AGTDTS_1 "ERR-AGTDTS-01: Can't find Telos Decide treasury for given symbol code: "

// aux_is_token_blacklisted
#define ERROR_AITB_1 "ERR-AITB-01: Token is not registered: " 

// handler_ballot_result_for_setcurrency
#define ERROR_HBRFSC_1 "ERR-HBRFSC-01: Token is not registered: " 
#define ERROR_HBRFSC_2 "ERR-HBRFSC-02: Token's contract differs from given contract: " 

// aux_process_ballot_to_ban_token
#define ERROR_APBTBT_1 "ERR-APBTBT-01: This token can't be banned because it belongs to the DEX token whitelist: " 
#define ERROR_APBTBT_2 "ERR-APBTBT-02: Token contract does not has a stats table: " 
#define ERROR_APBTBT_3 "ERR-APBTBT-03: This token is already blacklisted: " 

// handler_ballot_result
#define ERROR_HBR_1 "ERR-HBR-01: could't find a ballot identified: "
#define ERROR_HBR_2 "ERR-HBR-02: property not valid: "

// handler_ballot_result_for_bantoken
#define ERROR_HBRFB_1 "ERR-HBRFB-01: Symbol is registered in whitelist, can not be baned: "
#define ERROR_HBRFB_2 "ERR-HBRFB-02: Symbol is not part of the token list: "

// handler_ballot_result_for_savetoken
#define ERROR_HBRFS_1 "ERR-HBRFS-01: token is blacklisted and can not be saved: "
#define ERROR_HBRFS_2 "ERR-HBRFS-02: Symbol mismatch: "


