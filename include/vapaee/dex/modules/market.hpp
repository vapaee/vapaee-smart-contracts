#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/pool/tables.hpp>
#include <vapaee/pool/modules/swap.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <algorithm>

namespace vapaee {
    namespace utils {
        uint128_t pack_symbols_in_uint128(symbol_code code1, symbol_code code2);
    };

    namespace dex {
        namespace market {
            uint128_t aux_get_canonical_index_for_symbols(const symbol_code & A, const symbol_code & B);
        }
    };
};
  
namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace market {

            inline name get_self() {
                return vapaee::dex::contract;
            }


            string aux_get_market_name(uint64_t market_id) {
                // PRINT("vapaee::dex::market::aux_get_market_name()\n");
                markets mktable(get_self(), get_self().value);
                auto market = mktable.get(market_id,  create_error_id1(ERROR_AGTFM_1, market_id).c_str());
                return market.to_string();
            }

            // converters --------------------------------------------------------------------
            bool aux_check_converter_is_valid(uint64_t market_id, name converter) {
                PRINT("vapaee::dex::market::aux_check_converter_is_valid()\n");
                PRINT(" market_id: ", std::to_string((long)market_id)," \n");
                PRINT(" converter: ", converter.to_string()," \n");
                converters ctable(get_self(), get_self().value);
                auto market_index = ctable.get_index<"market"_n>();
                auto itr = market_index.find(market_id);
                check(itr != market_index.end(), create_error_id1(ERROR_ACCIV_1, market_id).c_str());
                
                // iterate over all converters looking for the one with the given name
                while (itr != market_index.end() && itr->market_id == market_id) {
                    if (itr->converter == converter) {
                        return true;
                    }
                    itr++;
                }

                check(false, create_error_string2(ERROR_ACCIV_2, std::to_string((long)market_id), converter.to_string()).c_str());

                return false;
            }

            uint64_t aux_get_converter_id(uint64_t market_id, name converter) {
                PRINT("vapaee::dex::market::aux_get_converter_id()\n");
                PRINT(" market_id: ", std::to_string((long)market_id)," \n");
                PRINT(" converter: ", converter.to_string()," \n");

                converters ctable(get_self(), get_self().value);
                auto market_index = ctable.get_index<"market"_n>();
                auto itr = market_index.find(market_id);
                check(itr != market_index.end(), create_error_id1(ERROR_AGCI_1, market_id).c_str());
                
                // iterate over all converters looking for the one with the given name
                while (itr != market_index.end()) {
                    if (itr->converter == converter) {
                        return itr->id;
                    }
                    itr++;
                }

                check(false, create_error_string2(ERROR_AGCI_2, std::to_string((long)market_id), converter.to_string()).c_str());

                return 0;
            }            

            void aux_add_market_converter(uint64_t market_id, name converter) {
                PRINT("vapaee::dex::market::create_default_converter()\n");
                PRINT(" market_id: ", std::to_string((long)market_id)," \n");

                converters ctable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto market_index = ctable.get_index<"market"_n>();
                auto itr = market_index.find(market_id);

                while (itr != market_index.end()) {
                    if (itr->converter == converter) {
                        return;
                    }
                    itr++;
                }

                ctable.emplace(vapaee::dex::contract, [&](auto &a){
                    a.id = ctable.available_primary_key();
                    a.market_id = market_id;
                    a.admin = vapaee::dex::contract;
                    a.converter = converter;
                    a.market_name = vapaee::dex::market::aux_get_market_name(market_id);
                });

                PRINT("vapaee::dex::market::create_default_converter() ...\n");
            }

            void aux_update_converter_state( uint64_t market_id, name converter) {
                PRINT("vapaee::dex::market::aux_update_converter_state()\n");
                PRINT(" market_id: ", std::to_string((long)market_id)," \n");
                PRINT(" converter: ", converter.to_string()," \n");
                
                uint64_t converter_id = aux_get_converter_id(market_id, converter);

                // get the table pools from the converter account and find the market pool
                vapaee::pool::pools ptable(converter, converter.value);
                auto pitr = ptable.find(market_id);

                check(pitr != ptable.end(), create_error_string2(ERROR_AUCS_1, std::to_string((long)market_id), converter.to_string()).c_str());
                asset fee = pitr->fee;
                asset currency = pitr->currency_reserve;
                asset commodity = pitr->commodity_reserve;

                asset price = vapaee::pool::swap::get_pool_price(market_id, converter);
                asset inverse = vapaee::pool::swap::get_pool_rate(market_id, converter);

                converters ctable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto citr = ctable.find(converter_id);
                check(citr != ctable.end(), create_error_id1(ERROR_AUCS_2, converter_id).c_str());

                ctable.modify(citr, same_payer, [&](auto &a){
                    a.state.fee = fee;
                    a.state.currency = currency;
                    a.state.commodity = commodity;
                    a.state.price = price;
                    a.state.inverse = inverse;
                    a.date = time_point_sec(current_time_point());
                });
            }

            // --------------------------------------------------------------------------------
            uint128_t aux_get_canonical_index_for_symbols(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_get_canonical_index_for_symbols()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                uint128_t index;
                uint128_t index_AB = vapaee::utils::pack_symbols_in_uint128(A, B); // normal
                uint128_t index_BA = vapaee::utils::pack_symbols_in_uint128(B, A);

                PRINT(" index_AB: ", std::to_string((unsigned long)index_AB), "\n");
                PRINT(" index_BA: ", std::to_string((unsigned long)index_BA), "\n");
                
                vapaee::dex::tokens tokenstable(get_self(), get_self().value);
                auto token_A = tokenstable.find(A.raw());
                auto token_B = tokenstable.find(B.raw());

                check((token_A->currency + token_B->currency) > 0,
                    create_error_string2(ERROR_AGCIFS_1, A.to_string(), B.to_string()).c_str());

                //*/
                // this is the new simplest version
                if (token_A->currency == 0) {
                    // A is a commodity token
                    index = index_AB;
                } else if (token_B->currency == 0) {
                    // B is a commodity token
                    index = index_BA;
                } else {
                    // If we get here, we have a currency pair
                    if (token_A->currency > token_B->currency) {
                        // A will be the commodity because is the newer one
                        index = index_AB;
                    } else {
                        // B will be the commodity because is the newer one
                        index = index_BA;
                    }
                }

                PRINT(" -> index: ", std::to_string((unsigned long)index), "\n");

                PRINT("SWAPPED: ", index == index_BA, "\n");
                // PRINT("vapaee::dex::market::aux_get_canonical_index_for_symbols() ...\n");
                return index;
            } 

            bool aux_is_allowed_to_create_market(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_is_it_allowed_to_create_this_market()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");

                vapaee::dex::tokens tokenstable(get_self(), get_self().value);
                auto atk_itr = tokenstable.find(A.raw());
                auto btk_itr = tokenstable.find(B.raw());
                
                check(atk_itr != tokenstable.end(), create_error_symcode1(ERROR_AIIATCTM_1, A).c_str());
                check(btk_itr != tokenstable.end(), create_error_symcode1(ERROR_AIIATCTM_2, B).c_str());
                check(atk_itr != btk_itr,           create_error_symcode1(ERROR_AIIATCTM_3, A).c_str());

                bool a_tradeable = atk_itr->tradeable;
                bool b_tradeable = btk_itr->tradeable;
                bool a_blacklisted = vapaee::dex::security::aux_is_token_blacklisted(A, atk_itr->contract);
                bool b_blacklisted = vapaee::dex::security::aux_is_token_blacklisted(B, btk_itr->contract);
                bool a_currency_in_b_groups = atk_itr->currency > 0;
                bool b_currency_in_a_groups = btk_itr->currency > 0;

                bool allowed =
                    a_tradeable &&
                    b_tradeable &&
                    !a_blacklisted &&
                    !b_blacklisted &&
                    (a_currency_in_b_groups || b_currency_in_a_groups);

                PRINT(" > a_tradeable: ", std::to_string(a_tradeable), "\n");
                PRINT(" > b_tradeable: ", std::to_string(b_tradeable), "\n");
                PRINT(" > a_blacklisted: ", std::to_string(a_blacklisted), "\n");
                PRINT(" > b_blacklisted: ", std::to_string(b_blacklisted), "\n");
                PRINT(" > a_currency_in_b_groups: ", std::to_string(a_currency_in_b_groups), "\n");
                PRINT(" > b_currency_in_a_groups: ", std::to_string(b_currency_in_a_groups), "\n");
                PRINT(" -->> allowed: ", std::to_string(allowed), "\n");

                return allowed;
            }

            uint64_t aux_create_market_and_return_canonical_id(const symbol_code& A, const symbol_code& B) {
                PRINT("vapaee::dex::market::aux_create_market_and_return_canonical_id()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                markets mktable(get_self(), get_self().value);

                // Is it allowed to create this market?
                check(
                    aux_is_allowed_to_create_market(A, B),
                    create_error_symcode2(ERROR_ACMARCI_1, A, B).c_str());
            
                symbol_code commodity = A;
                symbol_code currency = B;
                uint64_t id = vapaee::dex::global::get().next_market;
                
                uint128_t index_canonical = aux_get_canonical_index_for_symbols(A, B);
                uint128_t index_b = vapaee::utils::pack_symbols_in_uint128(A, B);

                if (index_canonical != index_b) {
                    commodity = B;
                    currency = A;
                }

                // emplace canonical market
                PRINT("  mktable.emplace() id\n", to_string((unsigned) id), 
                    ": ", commodity, "/", currency, "\n");
                mktable.emplace(get_self(), [&](auto & a){
                    a.id = id;
                    a.commodity = commodity;
                    a.currency = currency;
                });

                // emplace inverse market
                uint128_t index_inv = vapaee::utils::pack_symbols_in_uint128(currency, commodity);
                PRINT("  mktable.emplace() id+1\n", std::to_string((unsigned) id + 1), 
                    ": ", currency, "/", commodity, "\n");
                mktable.emplace(get_self(), [&](auto & a){
                    a.id = id + 1;
                    a.commodity = currency;
                    a.currency = commodity;
                });

                state newstate = vapaee::dex::global::get();
                newstate.next_market = id + 2;
                vapaee::dex::global::set(newstate);

                PRINT("vapaee::dex::market::aux_create_market_and_return_canonical_id() ...\n");
                return id;
            }

            /**
            * @breif: Returns the market id for the given symbols. If the market does not exist, fails.                
            * @param A First symbol.
            * @param B Second symbol.
            * @returns The market id.
            */
            uint64_t aux_get_market_id(const symbol_code& A, const symbol_code& B) {
                PRINT("vapaee::dex::market::aux_get_market_id()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                markets mktable(get_self(), get_self().value);
                auto tkn_index = mktable.get_index<"tokensidx"_n>();

                uint128_t index = vapaee::utils::pack_symbols_in_uint128(A, B);
                auto market = tkn_index.find(index);
                if(market != tkn_index.end()) {
                    PRINT("RET INDEX: ", market->id, "\n");
                    // PRINT("vapaee::dex::market::aux_get_market_id() ...\n");
                    return market->id;
                } else {
                    check(false, create_error_symcode2(ERROR_AGMI_1, A, B).c_str());
                }
                // unseachable code
                return 0;
            }

            /**
            * @breif: Returns the market id for the given symbols. If the market does not exist, creates it.
            * @param A First symbol.
            * @param B Second symbol.
            * @returns The market id.
            */            
            uint64_t aux_get_or_create_market_id(const symbol_code& A, const symbol_code& B) {
                PRINT("vapaee::dex::market::aux_get_or_create_market_id()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                markets mktable(get_self(), get_self().value);
                auto tkn_index = mktable.get_index<"tokensidx"_n>();

                uint128_t index = vapaee::utils::pack_symbols_in_uint128(A, B);
                auto market = tkn_index.find(index);
                if(market != tkn_index.end()) {
                    PRINT("RET INDEX: ", market->id, "\n");
                    PRINT("vapaee::dex::market::aux_get_or_create_market_id() ...\n");
                    return market->id;
                }

                uint64_t canonical_id = aux_create_market_and_return_canonical_id(A, B);

                market = tkn_index.find(index);
                check(market != tkn_index.end(), 
                    create_error_symcode2(ERROR_AGOCMI_1, A, B).c_str());
                uint64_t id = market->id;

                PRINT("RET INDEX: ", id, "\n");
                PRINT("vapaee::dex::market::aux_get_or_create_market_id() ...\n");
                return id;
            }

            uint64_t aux_get_canonical_market_id(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_get_canonical_market_id()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                
                uint128_t index_a = aux_get_canonical_index_for_symbols(A, B);
                uint128_t index_b = vapaee::utils::pack_symbols_in_uint128(A, B);
                if (index_a == index_b)
                    return aux_get_market_id(A, B);
                else
                    return aux_get_market_id(B, A);
            }

            uint64_t aux_get_inverted_market(const symbol_code & A, const symbol_code & B) {
                uint128_t index_a = aux_get_canonical_index_for_symbols(A, B);
                uint128_t index_b = vapaee::utils::pack_symbols_in_uint128(A, B);
                if (index_a != index_b)
                    return aux_get_market_id(A, B);
                else
                    return aux_get_market_id(B, A);
            }

            void action_newmarket(const symbol_code & token_a, const symbol_code & token_b, name converter) {
                // viterbotelos, sell, ACORN, TELOSD, [1]
                PRINT("vapaee::dex::market::action_newmarket()\n");
                PRINT(" token_a: ",  token_a.to_string(), "\n");
                PRINT(" token_b: ",  token_b.to_string(), "\n");
                PRINT(" converter: ", converter.to_string(), "\n");

                require_auth(vapaee::dex::contract);
                uint64_t market_id = aux_get_or_create_market_id(token_a, token_b);
                aux_add_market_converter(market_id, converter);

                PRINT("vapaee::dex::market::action_newmarket() ...\n");
            }
        };
    };
};
