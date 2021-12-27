#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <algorithm>


using vapaee::utils::symbols_get_index;


namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace market {

            bool aux_contains(vector<uint64_t> v, uint64_t x) {
                return std::find(v.begin(), v.end(), x) != v.end();
            }

            bool aux_is_A_currency_in_any_B_groups(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_is_A_currency_in_any_B_groups()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                
                tokens tokenstable(contract, contract.value);
                auto atk_itr = tokenstable.find(A.raw());
                auto btk_itr = tokenstable.find(B.raw());

                tokengroups groupstable(contract, contract.value);

                for (int i=0; i<btk_itr->groups.size(); i++) {
                    uint64_t group = btk_itr->groups[i];

                    auto ptr = groupstable.find(group);
                    check(ptr != groupstable.end(), create_error_string2(ERROR_AIACIABG_1, B.to_string(), std::to_string((unsigned long)group)).c_str());

                    for (int j=0; j<ptr->currencies.size(); j++) {
                        symbol_code currency = ptr->currencies[j];

                        if (currency == A) {
                            for (int k=0; k<atk_itr->groups.size(); k++) {
                                if (atk_itr->groups[k] == group) {
                                    return true;
                                }
                            }
                        }
                    }
                }

                return false;
                PRINT("vapaee::dex::market::aux_is_A_currency_in_any_B_groups() ...\n");
            }

            uint128_t aux_get_canonical_index_for_symbols(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_get_canonical_index_for_symbols()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                uint128_t index;
                uint128_t index_AB = symbols_get_index(A, B);
                uint128_t index_BA = symbols_get_index(B, A);

                tokens tokenstable(contract, contract.value);
                auto token_A = tokenstable.find(A.raw());
                auto token_B = tokenstable.find(B.raw());

                // if TLOS is one of them is the base token
                if (B == vapaee::utils::SYS_TKN_CODE)
                    index = index_AB;
                else if (A == vapaee::utils::SYS_TKN_CODE)
                    index = index_BA;
                else {
                    // let's see if one of them is currency in token group zero
                    if (token_A->currency && !token_B->currency)
                        index = index_BA;
                    else if (token_B->currency && !token_A->currency)
                        index = index_AB;
                    else {
                        // let's see if one of them is currency for the other one in any token group
                        bool A_is_currency_for_B = aux_is_A_currency_in_any_B_groups(A, B);
                        bool B_is_currency_for_A = aux_is_A_currency_in_any_B_groups(B, A);
                        if (A_is_currency_for_B && !B_is_currency_for_A)
                            index = index_BA;
                        else if (!A_is_currency_for_B && B_is_currency_for_A)
                            index = index_AB;
                        else {
                            // At this point this market should not be allowed unless both tokens are currencies at the same time
                            // in that las case we need to answer so let's do it alphabetically
                            // PS: in this function we don't care if the market can be created.
                            if (A.to_string() < B.to_string())
                                index = index_AB;
                            else
                                index = index_BA;
                        }
                    }
                }

                PRINT("SWAPPED: ", index == index_BA, "\n");

                PRINT("vapaee::dex::market::aux_get_canonical_index_for_symbols() ...\n");
                return index;
            } 

            bool aux_is_allowed_to_create_market(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_is_it_allowed_to_create_this_market()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");

                tokens tokenstable(contract, contract.value);
                auto atk_itr = tokenstable.find(A.raw());
                auto btk_itr = tokenstable.find(B.raw());
                
                check(atk_itr != tokenstable.end(), create_error_symcode1(ERROR_AIIATCTM_1, A).c_str());
                check(btk_itr != tokenstable.end(), create_error_symcode1(ERROR_AIIATCTM_2, B).c_str());
                check(atk_itr != btk_itr,           create_error_symcode1(ERROR_AIIATCTM_3, A).c_str());

                PRINT("vapaee::dex::market::aux_is_it_allowed_to_create_this_market()...\n");

                return
                    atk_itr->tradeable &&
                    btk_itr->tradeable &&
                    !vapaee::dex::security::aux_is_token_blacklisted(A, atk_itr->contract) &&
                    !vapaee::dex::security::aux_is_token_blacklisted(B, btk_itr->contract) &&
                    (aux_is_A_currency_in_any_B_groups(A, B) ||
                        aux_is_A_currency_in_any_B_groups(B, A));
            }

            uint64_t aux_create_market_and_return_canonical_id(const symbol_code& A, const symbol_code& B) {
                PRINT("vapaee::dex::market::aux_create_market_and_return_canonical_id()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                markets mktable(contract, contract.value);

                // Is it allowed to create this market?
                check(
                    aux_is_allowed_to_create_market(A, B),
                    create_error_symcode2(ERROR_ACMARCI_1, A, B).c_str());
            
                symbol_code commodity = A;
                symbol_code currency = B;
                uint64_t id = vapaee::dex::global::get().next_market;
                
                uint128_t index_canonical = aux_get_canonical_index_for_symbols(A, B);
                uint128_t index_b = symbols_get_index(A, B);

                if (index_canonical != index_b) {
                    commodity = B;
                    currency = A;
                }

                // emplace canonical market
                PRINT("  mktable.emplace() id\n", to_string((unsigned) id), 
                    ": ", commodity, "/", currency, "\n");
                mktable.emplace(contract, [&](auto & a){
                    a.id = id;
                    a.commodity = commodity;
                    a.currency = currency;
                });

                // emplace inverse market
                uint128_t index_inv = symbols_get_index(currency, commodity);
                PRINT("  mktable.emplace() id+1\n", std::to_string((unsigned) id + 1), 
                    ": ", currency, "/", commodity, "\n");
                mktable.emplace(contract, [&](auto & a){
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

            uint64_t aux_get_market_id(const symbol_code& A, const symbol_code& B) {
                PRINT("vapaee::dex::market::aux_get_market_id()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                markets mktable(contract, contract.value);
                auto tkn_index = mktable.get_index<"tokensidx"_n>();

                uint128_t index = symbols_get_index(A, B);
                auto market = tkn_index.find(index);
                if(market != tkn_index.end()) {
                    PRINT("RET INDEX: ", market->id, "\n");
                    PRINT("vapaee::dex::market::aux_get_market_id() ...\n");
                    return market->id;
                }

                uint64_t canonical_id = aux_create_market_and_return_canonical_id(A, B);

                market = tkn_index.find(index);
                check(market != tkn_index.end(), 
                    create_error_symcode2(ERROR_AGMI_1, A, B).c_str());
                uint64_t id = market->id;

                PRINT("RET INDEX: ", id, "\n");
                PRINT("vapaee::dex::market::aux_get_market_id() ...\n");
                return id;
            }            

            uint64_t aux_get_canonical_market(const symbol_code & A, const symbol_code & B) {
                uint128_t index_a = aux_get_canonical_index_for_symbols(A, B);
                uint128_t index_b = symbols_get_index(A, B);
                if (index_a == index_b)
                    return aux_get_market_id(A, B);
                else
                    return aux_get_market_id(B, A);
            }

            uint64_t aux_get_inverted_market(const symbol_code & A, const symbol_code & B) {
                uint128_t index_a = aux_get_canonical_index_for_symbols(A, B);
                uint128_t index_b = symbols_get_index(A, B);
                if (index_a != index_b)
                    return aux_get_market_id(A, B);
                else
                    return aux_get_market_id(B, A);
            }

            string aux_get_market_repr(uint64_t market_id) {
                markets mktable(contract, contract.value);
                auto market = mktable.get(market_id,  create_error_id1(ERROR_AGTFM_1, market_id).c_str());
                return market.repr();
            }

            void action_newmarket(const symbol_code & token_a, const symbol_code & token_b) {
                // viterbotelos, sell, ACORN, TELOSD, [1]
                PRINT("vapaee::dex::market::action_newmarket()\n");
                PRINT(" token_a: ",  token_a.to_string(), "\n");
                PRINT(" token_b: ",  token_b.to_string(), "\n");

                aux_get_market_id(token_a, token_b);

                PRINT("vapaee::dex::market::action_newmarket() ...\n");
            }
        };
    };
};
