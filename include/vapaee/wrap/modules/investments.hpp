#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/pool/tables.hpp>
#include <vapaee/wrap/tables.hpp>
#include <vapaee/pool/modules/liquidity.hpp>
#include <vapaee/wrap/modules/maintenance.hpp>

namespace vapaee {
    namespace wrap {
        namespace investments {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            void handle_telos_profits(asset quantity) {
                PRINT("vapaee::wrap::investments::handle_telos_profits()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // check if quantity is in TLOS
                check(quantity.symbol == vapaee::utils::SYS_TKN_SYMBOL, 
                    create_error_asset1(ERROR_HTP_1, quantity).c_str());

                // TODO: this must be part of a global config
                double percent_vapaee = 0.1; // percent to send to VAPAEE holders
                double percent_tlosv  = 0.9; // percent to buy TLOSV and send them back as profits
                asset asset_vapaee = asset(quantity.amount * percent_vapaee, quantity.symbol);
                asset asset_tlosv  = asset(quantity.amount * percent_tlosv, quantity.symbol);

                // create an action to transfer profits to VAPAEE holders
                string memo = string("profits,VAPAEE");
                name to = vapaee::pay::contract;
                vapaee::wrap::maintenance::do_schedule_transfer(to, asset_vapaee, memo);

                // create an action buy TLOSV and send them back as profits
                name swap = vapaee::pool::contract;
                name me = vapaee::wrap::contract;
                to = vapaee::echo::contract;
                string path = me.to_string()+"/TLOSW "+swap.to_string()+"/TLOSV";
                memo = string("openpool.v1,")+path+",0.0000 TLOSV,"+me.to_string()+",profits" ;
                vapaee::wrap::maintenance::do_schedule_transfer(to, asset_tlosv, memo);
            }

            void send_telosv_as_pool_profits(asset quantity) {
                PRINT("vapaee::wrap::investments::send_telosv_as_pool_profits()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // check if quantity is in TLOSV
                check(quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                    create_error_asset1(ERROR_STAPP_1, quantity).c_str());

                pools pool_table(vapaee::pool::contract, vapaee::pool::contract.value);
                uint64_t main_pool_id = vapaee::pool::liquidity::get_main_pool_id();
                auto mainpool = pool_table.find(main_pool_id);

                // create an action to transfer the profits to the main pool
                string memo = string("profits,") + mainpool->to_string();
                name to = vapaee::pool::contract;
                name from = get_self();

                vapaee::wrap::maintenance::do_schedule_transfer(to, quantity, memo);
            }


            /**
            * @brief this functions iterate over the existing pools and performs an investment on each.
            * the amount of investment on each pool depends on the current size (liquidity) of the pool over the total.
            * Main pool (TLOSV/TLOSW) is skiped and any other pool not having TLOSV as one of the tokens is skipped too
            * 
            * @param quantity the amount of TLOSV to invest in total
            */
            void send_telosv_to_swapfund_pools(asset quantity) {
                PRINT("vapaee::wrap::investments::send_telosv_to_swapfund_pools()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // check if quantity is in TLOSV
                check(quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                    create_error_asset1(ERROR_STTSP_1, quantity).c_str());         

                pools pool_table(vapaee::pool::contract, vapaee::pool::contract.value);

                // get the id for pool TLOSV/TLOSW
                uint64_t main_pool_id = vapaee::pool::liquidity::get_main_pool_id();

                // initialice total reserve in zero
                asset total_reserve = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);

                // get a pointer to the pool tables and iterate over the pools
                for (
                    auto pool_it = pool_table.begin();
                    pool_it != pool_table.end();
                    pool_it++
                ) {
                    // if the pool is TLOSV/TLOSW then skip it
                    if (pool_it->market_id == main_pool_id) continue;

                    // if the pool does not hav TLOSV as currency then skip it
                    if (pool_it->currency_reserve.symbol != vapaee::wrap::TLOSV_TKN_SYMBOL) continue;

                    // else add currency_reserve to total participation and store pool percent over total
                    total_reserve.amount += pool_it->currency_reserve.amount;
                }

                // iterate over added pools and create a transfer to swapfund for each pool
                asset remaining = quantity;
                for (
                    auto pool_it = pool_table.begin();
                    pool_it != pool_table.end();
                    pool_it++
                ) {
                    // if the pool is TLOSV/TLOSW then skip it
                    if (pool_it->market_id == main_pool_id) continue;

                    // if the pool does not hav TLOSV as currency then skip it
                    if (pool_it->currency_reserve.symbol != vapaee::wrap::TLOSV_TKN_SYMBOL) continue;

                    // calculate the percent participation of the pool
                    double percent = (double)pool_it->currency_reserve.amount / (double)total_reserve.amount;

                    // schedule a transfer to swapfund
                    asset investment = asset(quantity.amount * percent, quantity.symbol);
                    
                    // do not send more than what we have
                    if (investment.amount > remaining.amount) {
                        investment = remaining;
                    }

                    // prepare and shedule the transfer
                    name to = vapaee::pool::contract;
                    string memo = "swapfund,"+pool_it->to_string();
                    vapaee::wrap::maintenance::do_schedule_transfer(to, investment, memo);
                }
            }


            /**
            * @brief this function receives some amount of TLOSV and performs a transformation to TLOSW.
            * This is done by delegating some old loans that has not been updated recently.
            * This action prevents lons from beeing locked forever and pushes up the TLOSV's price.
            * @param tlosv Amount of TLOSV to spend.
            * @returns Amount of TLOSV remaining not spent.
            */            
            /*asset spend_some_tlosv_to_delegate_oldest_loan (asset tlosv) {
                // PRINT("vapaee::wrap::investments::spend_some_tlosv_to_delegate_oldest_loan()\n");
                // PRINT(" tlosv: ", tlosv.to_string(), "\n");

                // check if tlosv is in TLOSV
                check(tlosv.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                    create_error_asset1(ERROR_SSTAVOL_1, tlosv).c_str()); 

                // 10% but no more than 10.0000 TLOSV
                asset tospend = asset( vapaee::utils::max(10000, tlosv.amount * 0.1),  tlosv.symbol);
                asset remaining = tlosv;
                asset spent = vapaee::wrap::lendings::delegate_oldest_loan(tospend);
                if (spent.amount > 0) {
                    remaining = tlosv - spent;

                    // burn the TLOSV
                    action(
                        permission_level{get_self(), "active"_n},
                        vapaee::token::contract,
                        "burn"_n,
                        std::make_tuple(vapaee::wrap::contract, tospend, "TLOSV spent to age oldest loan")
                    ).send();

                    // change ths symbol to TLOSW
                    tospend.symbol = vapaee::wrap::TLOSW_TKN_SYMBOL;

                    // isssue TLOSW
                    action(
                        permission_level{get_self(), "active"_n},
                        vapaee::token::contract,
                        "issue"_n,
                        std::make_tuple(vapaee::wrap::contract, tospend, "TLOSW issued to age oldest loan")
                    ).send();

                    // send TLOSW to profits
                    vapaee::wrap::maintenance::do_schedule_transfer(vapaee::echo::contract, tospend, "profits");

                }
                return remaining;
            }*/

            void handle_tlosv_profits(asset tlosv) {
                PRINT("vapaee::wrap::investments::handle_tlosv_profits()\n");
                PRINT(" tlosv: ", tlosv.to_string(), "\n");

                // check if tlosv is in TLOSV
                check(tlosv.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                    create_error_asset1(ERROR_HTVP_1, tlosv).c_str());         

                // TODO: this must be part of a global config
                double percent_profits = 0.4; // percent to main pool investors as profits
                double percent_invest  = 0.6; // percent to invest in pools

                asset asset_profits = asset(tlosv.amount * percent_profits, tlosv.symbol);
                asset asset_invest  = asset(tlosv.amount * percent_invest,  tlosv.symbol);

                // send TLOSV to the main pool as profits for investors
                send_telosv_as_pool_profits(asset_profits);

                // send TLOSV to swapfund. Add any remaining TLOSV from first action
                send_telosv_to_swapfund_pools(asset_invest);
            }            

            void handle_tlosw_profits(asset quantity) {
                PRINT("vapaee::wrap::investments::handle_tlosw_profits()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // check if quantity is in TLOSW
                check(quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL, 
                    create_error_asset1(ERROR_HTWP_1, quantity).c_str());         

                // swap TLOSW for TLOSV and put "profits" in the memo
                name to = vapaee::pool::contract;
                name me = vapaee::wrap::contract;
                string memo = "openpool.v1,"+to.to_string()+"/TLOSV,0.0000 TLOSV,"+me.to_string()+",profits" ;
                vapaee::wrap::maintenance::do_schedule_transfer(to, quantity, memo);
            }

            void handle_custom_token_profits(asset quantity, name tokencontract) {
                PRINT("vapaee::wrap::investments::handle_token_profits()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // schedule a swapfund to the pool "<quantity.symbol>/TLOSV"
                name to = vapaee::pool::contract;
                string memo = "swapfund," + quantity.symbol.code().to_string() + "/TLOSV" ;
                vapaee::wrap::maintenance::do_schedule_transfer(to, quantity, memo);
            }

            void handle_profits(asset quantity, name tokencontract) {
                PRINT("vapaee::wrap::investments::handle_profits()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" tokencontract: ", tokencontract.to_string(), "\n");

                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HDT_5);
                
                // if the quantity is TLOS
                if (quantity.symbol == vapaee::utils::SYS_TKN_SYMBOL) {
                    handle_telos_profits(quantity);
                }

                // if the quantity is TLOSV
                else if (quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL) {
                    handle_tlosv_profits(quantity);
                }

                // if the quantity is TLOSW
                else if (quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL) {
                    handle_tlosw_profits(quantity);
                }
                
                // if the quantity is any other token
                else {
                    handle_custom_token_profits(quantity, tokencontract);
                }

            }



            // This quantity is part of the system profits
            // It is distributed according to the contract politics
            void action_profits(asset quantity) {
                PRINT("vapaee::wrap::investments::action_profits() ------------------\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                handle_profits(quantity, 
                    vapaee::dex::utils::get_contract_for_token(quantity.symbol.code()));
                    
            }

        };     
    };
};
