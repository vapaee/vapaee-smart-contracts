#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/utils.hpp>

using std::strtoull;

using vapaee::pool::utils::create_pool;

using vapaee::pool::utils::create_fund_attempt;
using vapaee::pool::utils::end_fund_attempt;
using vapaee::pool::utils::record_fund_attempt;
using vapaee::pool::utils::get_market_id_for_syms;
using vapaee::pool::utils::withdraw_participation;

using vapaee::utils::split;


namespace vapaee {
    namespace pool {

        using namespace global;

        namespace liquidity {

            inline name get_self() {
                return vapaee::pool::contract;
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

                fund_attempts funding_attempts(get_self(), funder.value);
                auto fund_it = funding_attempts.find(market_id);
                check(fund_it != funding_attempts.end(), ERR_ATTEMPT_NOT_FOUND);

                // cancel request is valid, return funds if any
                markets book_markets(vapaee::dex::contract, vapaee::dex::contract.value);
                auto book_it = book_markets.find(fund_it->market_id);
                check(book_it != book_markets.end(), ERR_MARKET_NOT_FOUND);

                if (fund_it->commodity.amount > 0)
                    action(
                        permission_level{get_self(), "active"_n},
                        get_contract_for_token(book_it->commodity),
                        "transfer"_n,
                        make_tuple(
                            get_self(), funder,
                            fund_it->commodity, "cancel fund attempt " + to_string(market_id)
                        )
                    ).send();

                if (fund_it->currency.amount > 0)
                    action(
                        permission_level{get_self(), "active"_n},
                        get_contract_for_token(book_it->currency),
                        "transfer"_n,
                        make_tuple(
                            get_self(), funder,
                            fund_it->currency, "cancel fund attempt " + to_string(market_id)
                        )
                    ).send();

                funding_attempts.erase(fund_it);
                PRINT("vapaee::pool::liquidity::action_cancel_fund() ...\n");
            }

            void fund(name from, asset quantity, string market_name) {
                PRINT("vapaee::pool::liquidity::fund()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" market_name: ", market_name.c_str(), "\n");

                // split second param, can be either numeric pool id or symbol pair
                vector<string> market_symbols = split(market_name, "/");

                uint64_t market_id;
                if (market_symbols.size() == 1) {
                    // numeric id
                    market_id = strtoull(market_name.c_str(), nullptr, 10);
                } else {
                    // symbol pair
                    get_market_id_for_syms(
                        symbol_code(market_symbols[0]),
                        symbol_code(market_symbols[1]),
                        &market_id);
                }

                // must be canonical market
                check(market_id % 2 == 0, ERR_MARKET_INVERSE);


                // get pool or create
                pools pool_markets(get_self(), get_self().value);
                auto pool_it = pool_markets.find(market_id);
                if (pool_it == pool_markets.end()) {

                    // check if the two market_symbols are commodity and currency or viceversa in the pool_it
                    if (market_symbols[0] == pool_it->commodity_reserve.symbol.code().to_string()) {
                        if (market_symbols[1] == pool_it->currency_reserve.symbol.code().to_string()) {
                            create_pool(market_id);
                            pool_it = pool_markets.find(market_id);
                        } else {
                            check(false, "ERROR: second market symbol is not the pool currency");
                        }
                    } else if (market_symbols[1] == pool_it->commodity_reserve.symbol.code().to_string()) {
                        if (market_symbols[0] == pool_it->currency_reserve.symbol.code().to_string()) {
                            create_pool(market_id);
                            pool_it = pool_markets.find(market_id);
                        } else {
                            check(false, "ERROR: first market symbol is not the pool currency");
                        }
                    } else {
                        check(false, "ERROR: first market symbol does not belong to this pool");
                    }
                    
                }

                fund_attempts funding_attempts(get_self(), from.value);
                auto fund_it = funding_attempts.find(market_id);

                if (fund_it == funding_attempts.end()) {
                    // create attempt record
                    create_fund_attempt(from, market_id);
                    fund_it = funding_attempts.find(market_id);
                }
                
                check(fund_it != funding_attempts.end(), ERR_ATTEMPT_NOT_FOUND);

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
                return;


            }
            
        };     
    };
};
