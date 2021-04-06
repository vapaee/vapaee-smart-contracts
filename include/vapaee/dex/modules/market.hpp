#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <algorithm>

namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace market {

            bool aux_contains(vector<uint64_t> v, uint64_t x) {
                return std::find(v.begin(), v.end(), x) != v.end();
            }

            bool aux_is_token_blacklisted(const symbol_code &sym_code, name contract) {
                PRINT("vapaee::dex::market::aux_is_token_blacklisted()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                PRINT(" contract: ", contract.to_string(), "\n");

                blacklist list(contract, contract.value); 
                auto index = list.get_index<name("symbol")>();
                auto itr = index.lower_bound(sym_code.raw());
                for (auto itr = index.lower_bound(sym_code.raw()); itr != index.end(); itr++) {
                    if (itr->symbol == sym_code) {
                        if (itr->contract == contract) {
                            PRINT("vapaee::dex::market::aux_is_token_blacklisted() ...\n");
                            return true;
                        }
                    } else {
                        break;
                    }
                }
                PRINT("vapaee::dex::market::aux_is_token_blacklisted() ...\n");
                return false;
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

            name aux_get_canonical_scope_for_symbols(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_get_canonical_scope_for_symbols()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                name scope;
                name scope_AB = aux_get_scope_for_tokens(A, B);
                name scope_BA = aux_get_scope_for_tokens(B, A);

                tokens tokenstable(contract, contract.value);
                auto tokenA = tokenstable.find(A.raw());
                auto tokenB = tokenstable.find(B.raw());                


                // if TLOS is one of them is the base token
                if (B == vapaee::utils::SYS_TKN_CODE) {
                    scope = scope_AB;
                } else if (A == vapaee::utils::SYS_TKN_CODE) {
                    scope = scope_BA;
                } else {
                    
                    // let's see if one of them is currency in token group zero
                    if (tokenA->currency && !tokenB->currency) {
                        scope = scope_BA;
                    } else if (tokenB->currency && !tokenA->currency) {
                        scope = scope_AB;
                    } else {
                        // let's see if one of them is currency for the other one in any token group
                        bool A_is_currency_for_B = aux_is_A_currency_in_any_B_groups(A, B);
                        bool B_is_currency_for_A = aux_is_A_currency_in_any_B_groups(B, A);
                        if (A_is_currency_for_B && !B_is_currency_for_A) {
                            scope = scope_BA;
                        } else if (A_is_currency_for_B && !B_is_currency_for_A) {
                            scope = scope_AB;
                        } else {
                            // At this point this market should not be allowed unless both tokens are currencies at the same time
                            // in that las case we need to answer so let's do it alphabetically
                            // PS: in this function we don't care if the market can be created.
                            if (A.to_string() < B.to_string()) {
                                scope = aux_get_scope_for_tokens(A, B);
                            } else {
                                scope = aux_get_scope_for_tokens(B, A);
                            }
                        }
                    }

                }

                PRINT(" ->scope: ", scope.to_string(), "\n");
                
                PRINT("vapaee::dex::market::aux_get_canonical_scope_for_symbols() ...\n");
                return scope;
            } 

            bool aux_is_it_allowed_to_cerate_this_market(const symbol_code & A, const symbol_code & B) {
                PRINT("vapaee::dex::market::aux_is_it_allowed_to_cerate_this_market()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");

                // TODO: debería fijarse si los tokens no están blacklisted y si el token es tradeable

                tokens tokenstable(contract, contract.value);
                auto atk_itr = tokenstable.find(A.raw());
                auto btk_itr = tokenstable.find(B.raw());
                
                check(atk_itr != tokenstable.end(), create_error_symcode1(ERROR_AIIATCTM_1, A).c_str());
                check(btk_itr != tokenstable.end(), create_error_symcode1(ERROR_AIIATCTM_2, B).c_str());
                check(atk_itr != btk_itr,           create_error_symcode1(ERROR_AIIATCTM_3, A).c_str());

                bool allowed = false;

                if (!atk_itr->tradeable || !btk_itr->tradeable) {
                    allowed = true;
                }

                if (aux_is_A_currency_in_any_B_groups(A, B)) {
                    allowed = true;
                }

                if (aux_is_A_currency_in_any_B_groups(B, A)) {
                    allowed = true;
                }

                PRINT("vapaee::dex::market::aux_is_it_allowed_to_cerate_this_market()...\n");
                return allowed;
            }

            void aux_create_market(const symbol_code& A, const symbol_code& B) {
                PRINT("vapaee::dex::market::aux_create_market()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                markets mktable(contract, contract.value);

                // Is it allowed to create this market?
                if (!aux_is_it_allowed_to_cerate_this_market(A,B)) {
                    check(false, create_error_symcode2(ERROR_ACMARI_1, A,B).c_str());
                }
            
                symbol_code commodity = A;
                symbol_code currency = B;
                uint64_t id = vapaee::dex::global::get().next_market;
                uint64_t market = id;
                name scope_canonical = aux_get_canonical_scope_for_symbols(A, B);
                name scope_b = aux_get_scope_for_tokens(A, B);

                if (scope_canonical != scope_b) {
                    commodity = B;
                    currency = A;
                    market++;
                }
                PRINT("  mktable.emplace() id\n", std::to_string((unsigned) id), scope_canonical.to_string(), "\n");
                mktable.emplace(contract, [&](auto & a){
                    a.id = id;
                    a.table = scope_canonical;
                    a.commodity = commodity;
                    a.currency = currency;
                });
                name scope_inv = aux_get_scope_for_tokens(currency, commodity);
                PRINT("  mktable.emplace() id+1\n", std::to_string((unsigned) id+1), scope_canonical.to_string(), "\n");
                mktable.emplace(contract, [&](auto & a){
                    a.id = id+1;
                    a.table = scope_inv;
                    a.commodity = currency;
                    a.currency = commodity;
                });

                state newstate = vapaee::dex::global::get();
                newstate.next_market = id + 2;
                vapaee::dex::global::set(newstate);

                PRINT("vapaee::dex::market::aux_create_market() ...\n");
            }

            uint64_t aux_get_market_id(const symbol_code& A, const symbol_code& B) {
                PRINT("vapaee::dex::market::aux_get_market_id()\n");
                PRINT(" A: ", A.to_string(), "\n");
                PRINT(" B: ", B.to_string(), "\n");
                markets mktable(contract, contract.value);
                auto index = mktable.get_index<name("table")>();

                name scope_b = aux_get_scope_for_tokens(A, B);
                
                for (auto itr = index.lower_bound(scope_b.value); itr != index.end(); itr++) {
                    if (itr->table == scope_b) {
                        if (itr->commodity == A && itr->currency == B) {
                            return itr->id;
                        }
                    } else {
                        break;
                    }
                }

                aux_create_market(A, B);
                uint64_t market = aux_get_market_id(A, B); // recursive call
                PRINT(" -> recursive call returned: ", std::to_string((unsigned long)market), "\n");
                PRINT("vapaee::dex::market::aux_get_market_id() ...\n");
                return market;
            }            

            uint64_t aux_get_canonical_market(const symbol_code & A, const symbol_code & B) {
                name scope_a = aux_get_canonical_scope_for_symbols(A, B);
                name scope_b = aux_get_scope_for_tokens(A, B);
                if (scope_a == scope_b) return aux_get_market_id(A, B);
                if (scope_a != scope_b) return aux_get_market_id(B, A);
                return 0;
            }

            uint64_t aux_get_inverted_market(const symbol_code & A, const symbol_code & B) {
                name scope_a = aux_get_canonical_scope_for_symbols(A, B);
                name scope_b = aux_get_scope_for_tokens(A, B);
                if (scope_a != scope_b) return aux_get_market_id(A, B);
                if (scope_a == scope_b) return aux_get_market_id(B, A);
                return 0;
            }

            name aux_get_table_from_market(uint64_t market_id) {
                PRINT("vapaee::dex::market::aux_get_table_from_market()\n");
                PRINT(" market_id: ", std::to_string((unsigned long) market_id), "\n");

                // if the market is being deleted, is not persent in the market table but it can be found in the delmarkets table
                delmarkets delmktable(contract, contract.value);
                auto market_ptr = delmktable.find(market_id);
                if (market_ptr != delmktable.end()) {
                    PRINT("vapaee::dex::market::aux_get_table_from_market()...\n");
                    return market_ptr->table;
                }

                markets mktable(contract, contract.value);
                auto market = mktable.get(market_id,  create_error_id1(ERROR_AGTFM_1, market_id).c_str());
                PRINT("vapaee::dex::market::aux_get_table_from_market()...\n");
                return market.table;
            }

        };
    };
};
