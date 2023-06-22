#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/utils.hpp>
#include <vapaee/pool/errors.hpp>
#include <vapaee/token/modules/utils.hpp>

using std::strtoull;

using vapaee::pool::utils::create_fund_attempt;
using vapaee::pool::utils::end_fund_attempt;
using vapaee::pool::utils::record_fund_attempt;
using vapaee::pool::utils::get_market_id_for_syms;
using vapaee::pool::utils::withdraw_participation;
using vapaee::pool::utils::extract_canonical_market_id_from_market_name;


using vapaee::utils::split;


namespace vapaee {
    namespace pool {

        // using namespace global;

        namespace liquidity {

            inline name get_self() {
                return vapaee::pool::contract;
            }

            uint64_t get_main_pool_id() {
                // PRINT("vapaee::pool::liquidity::get_main_pool_id()\n");
                // get the id for pool TLOSV/TLOSW
                pools pool_table(get_self(), get_self().value);
                auto sym_index = pool_table.get_index<"symbols"_n>();
                auto pool_index = vapaee::utils::pack_symbols_in_uint128(
                    vapaee::wrap::TLOSW_TKN_SYMBOL.code(), vapaee::wrap::TLOSV_TKN_SYMBOL.code());
                auto main_pool_it = sym_index.find(pool_index);
                uint64_t main_pool_id = main_pool_it->market_id;
                return main_pool_id;
            }


            void action_withdraw_participation(name funder, uint64_t market_id, asset score) {
                require_auth(funder);
                withdraw_participation(funder, market_id, score);
            }

            void action_cancel_fund(name funder, uint64_t market_id) {
                PRINT("vapaee::pool::liquidity::action_cancel_fund()\n");
                PRINT(" funder: ", funder.to_string(), "\n");
                PRINT(" market_id: ", market_id, "\n");

                require_auth(funder);

                fundattempts funding_attempts(get_self(), funder.value);
                auto fund_it = funding_attempts.find(market_id);
                check(fund_it != funding_attempts.end(), create_error_id1(ERROR_ACF_1, market_id).c_str());

                // cancel request is valid, return funds if any
                markets markets_table(vapaee::dex::contract, vapaee::dex::contract.value);
                auto book_it = markets_table.find(market_id);
                check(book_it != markets_table.end(), create_error_id1(ERROR_ACF_2, market_id).c_str());

                if (fund_it->commodity.amount > 0)
                    action(
                        permission_level{get_self(), "active"_n},
                        dex::utils::get_contract_for_token(book_it->commodity),
                        "transfer"_n,
                        make_tuple(
                            get_self(), funder,
                            fund_it->commodity, "cancel fund attempt " + to_string(market_id)
                        )
                    ).send();

                if (fund_it->currency.amount > 0)
                    action(
                        permission_level{get_self(), "active"_n},
                        dex::utils::get_contract_for_token(book_it->currency),
                        "transfer"_n,
                        make_tuple(
                            get_self(), funder,
                            fund_it->currency, "cancel fund attempt " + to_string(market_id)
                        )
                    ).send();

                funding_attempts.erase(fund_it);
                PRINT("vapaee::pool::liquidity::action_cancel_fund() ...\n");
            }

            void create_pool(uint64_t market_id) {
                vapaee::pool::utils::create_pool(market_id);
            }

            void aux_send_update_converter_state(uint64_t market_id, name converter) {
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    vapaee::dex::contract,
                    "updpoolswap"_n,
                    make_tuple(market_id, converter)
                ).send();
            }

            void fund_attempt(name from, asset quantity, string market_name) {
                PRINT("vapaee::pool::liquidity::fund_attempt()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" market_name: ", market_name.c_str(), "\n");

                // split second param, can be either numeric pool id or symbol pair
                vector<string> market_symbols = split(market_name, "/");
                uint64_t market_id = extract_canonical_market_id_from_market_name(market_name);

                // get pool or create
                markets marketstable(vapaee::dex::contract, vapaee::dex::contract.value);
                pools pool_markets(get_self(), get_self().value);
                auto mark_it = marketstable.find(market_id);
                auto pool_it = pool_markets.find(market_id);

                check(mark_it != marketstable.end(), create_error_id1(ERROR_FA_2, market_id).c_str());

                if (pool_it == pool_markets.end()) {

                    // check if the two market_symbols are commodity and currency or viceversa in the mark_it
                    if (market_symbols[0] == mark_it->commodity.to_string()) {
                        if (market_symbols[1] == mark_it->currency.to_string()) {
                            create_pool(market_id);
                            pool_it = pool_markets.find(market_id);
                        } else {
                            check(false, create_error_string2(ERROR_FA_3, market_symbols[1], mark_it->currency.to_string()).c_str());
                        }
                    } else if (market_symbols[1] == mark_it->commodity.to_string()) {
                        if (market_symbols[0] == mark_it->currency.to_string()) {
                            create_pool(market_id);
                            pool_it = pool_markets.find(market_id);
                        } else {
                            check(false, create_error_string2(ERROR_FA_4, market_symbols[0], mark_it->currency.to_string()).c_str());
                        }
                    } else {
                        check(false, create_error_string4(ERROR_FA_5,
                            market_symbols[0],
                            market_symbols[1],
                            mark_it->commodity.to_string(), 
                            mark_it->currency.to_string()
                        ).c_str());
                    }
                    
                }

                fundattempts funding_attempts(get_self(), from.value);
                auto fund_it = funding_attempts.find(market_id);
                if (fund_it == funding_attempts.end()) {
                    // create attempt record
                    create_fund_attempt(from, market_id);
                    fund_it = funding_attempts.find(market_id);
                }
                
                check(fund_it != funding_attempts.end(), create_error_id1(ERROR_FA_6, market_id).c_str());

                if (quantity.symbol == fund_it->commodity.symbol) {
                    funding_attempts.modify(fund_it, get_self(), [&](auto &row) {
                        row.commodity += quantity;
                    });
                }

                if (quantity.symbol == fund_it->currency.symbol) {
                    funding_attempts.modify(fund_it, get_self(), [&](auto &row) {
                        row.currency += quantity;
                    });
                }

                record_fund_attempt(market_id, from, quantity);

                if (fund_it->commodity.amount == 0 || fund_it->currency.amount == 0)
                    return;

                end_fund_attempt(from, market_id); 

                aux_send_update_converter_state(
                    market_id,
                    vapaee::pool::contract
                );

                return;


            }

            // add some quantity to a pool as profits (does not modify participation, just more liquidity)
            void add_profits(asset quantity, string market_name) {
                uint64_t market_id = extract_canonical_market_id_from_market_name(market_name);

                // get pool (must exist)
                pools pool_markets(get_self(), get_self().value);
                auto pool_it = pool_markets.find(market_id);
                check(pool_it != pool_markets.end(), create_error_id1(ERROR_AP_1, market_id).c_str());

                pool_markets.modify(pool_it, same_payer, [&](auto &row) {
                    if (row.commodity_reserve.symbol == quantity.symbol) {
                        row.commodity_reserve += quantity;
                    } else if (row.currency_reserve.symbol == quantity.symbol) {
                        row.currency_reserve += quantity;
                    } else {
                        check(false, create_error_asset3(
                            ERROR_AP_2,
                            quantity,
                            row.commodity_reserve,
                            row.currency_reserve
                        ).c_str());
                    }
                });

            }
            
        };     
    };
};
