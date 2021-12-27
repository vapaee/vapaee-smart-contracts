#pragma once


// Market Module ----------------------

// aux_is_A_currency_in_any_B_groups
#define ERROR_AIACIABG_1 "ERR-AIACIABG-01: Can't find tokengroup for token: "

// aux_is_it_allowed_to_cerate_this_market
#define ERROR_AIIATCTM_1 "ERR-AIIATCTM-01: This token is not registered: "
#define ERROR_AIIATCTM_2 "ERR-AIIATCTM-02: This token is not registered: "
#define ERROR_AIIATCTM_3 "ERR-AIIATCTM-03: Can't create market. Both tokens are the same: "

// aux_get_table_from_market
#define ERROR_AGTFM_1 "ERR-AGTFM-01: not market found: "

// aux_create_market_and_return_canonical_id
#define ERROR_ACMARCI_1 "ERR-ACMARCI-01: This market is not allowed because none of them are currencies and they are not in the same token group: "

// aux_get_market_id
#define ERROR_AGMI_1 "ERR-AGMI-01: Market creation failed: "


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

// utils get_contract_for_token
#define ERROR_UGCFT_1 "ERR-UGCFT-01: Token not found"

// utils aux_check_token_ok
#define ERROR_ACTOK_1 "Token not registered"
#define ERROR_ACTOK_2 "Token may be fake. Registered token countract does not match the current token contract"
#define ERROR_ACTOK_3 "Token is not tradeable"
#define ERROR_ACTOK_4 "Token is blacklisted"


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

// Fees Module ------------------------

// handle_dex_transfer
#define ERROR_HDT_1 "ERR-HDT-01: sender is not a valid account: "
#define ERROR_HDT_2 "ERR-HDT-02: you must send exactly the cost of a ballot for Telos Decide; (got)(needed):  "
#define ERROR_HDT_3 "ERR-HDT-03: you must send exactly the cost of Token registration; (got)(needed):  "
#define ERROR_HDT_4 "ERR-HDT-04: Invalid memo. Possible values are: 'ballot' and 'addtoken' but got"
#define ERROR_HDT_5 "ERR-HDT-05: "

// aux_delete_fees
#define ERROR_ADBF_1 "ERR-ADBF-01: fees for ballot could not be found for: "

// aux_add_fees
#define ERROR_AABF_1 "ERR-AABF-01: the accaunt already has deposited fee for that concept: "

