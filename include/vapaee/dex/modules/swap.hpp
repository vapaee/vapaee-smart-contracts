#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>

namespace vapaee {
    namespace pool {
        namespace util {
            void send_swap(
                const name& from,
                const asset& quantity,
                const symbol_code& token_to_receive,
                const name& recipiant,
                const string& memo,
                const name& converter,
                const string& path
            );
        };
    };
    namespace dex {
        namespace security {
            void aux_check_token_ok(const symbol& sym, name tokencontract, string error_code);
        };
        namespace market {
            uint64_t aux_get_market_id(const symbol_code& A, const symbol_code& B);
            uint64_t aux_get_canonical_market_id(const symbol_code& A, const symbol_code& B);
            bool aux_does_exist_market(const symbol_code& A, const symbol_code& B);
        };
    };
};
namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace swap {

            inline name get_self() {
                return vapaee::dex::contract;
            }

            name aux_get_any_conveter_for_market_id(uint64_t market_id) {
                PRINT("vapaee::dex::market::aux_get_any_conveter_for_market_id()\n");
                PRINT(" market_id: ", std::to_string((long)market_id)," \n");

                converters ctable(get_self(), get_self().value);
                auto market_index = ctable.get_index<"market"_n>();
                auto itr = market_index.find(market_id);
                check(itr != market_index.end(), create_error_id1(ERROR_GACF_1, market_id).c_str());
                
                return itr->converter;
            }

            void handle_start_swap_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::dex::swap::handle_start_swap_transfer()\n");

                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HSST_1);

                // memo format: "openpool.v1";token;recipiant;swapmemo;
                vector<string> parts = split(memo, ';');
                check(parts.size() == 4, create_error_string1(ERROR_HSST_2, memo).c_str());

                symbol_code token = vapaee::utils::check_symbol_code_from_string(parts[1]);
                name recipiant = vapaee::utils::check_name_from_string(parts[2]);
                string swapmemo = parts[3];

                // TODO: arreglar
                // hay que preguntar si alguno de los dos tokens es currency
                // si lo es, se ejecuta el código actual
                // si no lo es, habría que crear dos swaps, de Quantity a Currency y de Currency a Token

                symbol_code from_token = quantity.symbol.code();
                symbol_code to_token = token;
                bool market_exists = vapaee::dex::market::aux_does_exist_market(from_token, to_token);
                uint64_t market;
                name converter;
                string path;


                if (market_exists) {
                    market = vapaee::dex::market::aux_get_market_id(quantity.symbol.code(), token);
                    converter = aux_get_any_conveter_for_market_id(market);
                    path = converter.to_string()+"/"+token.to_string();
                } else {
                    /*
                    El algoritmo es el siguiente:
                    1 - iterar sobre los tokens existentes
                    2 - para cada token que sea diferente de from_token y to_token hacer:
                    2.1 - preguntar si existe un mercado entre from_token y token
                    2.2 - preguntar si existe un mercado entre token y to_token
                    2.3 - si ambos existen, entonces se encontró un camino
                    */
                    vapaee::dex::tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                    auto itr = tokenstable.begin();
                    while (itr != tokenstable.end()) {
                        symbol_code token = itr->symbol;
                        if (token != from_token && token != to_token) {
                            bool market1_ok = vapaee::dex::market::aux_does_exist_market(from_token, token);
                            bool market2_ok = vapaee::dex::market::aux_does_exist_market(token, to_token);
                            if (market1_ok && market2_ok) {
                                uint64_t market1 = vapaee::dex::market::aux_get_canonical_market_id(from_token, token);
                                uint64_t market2 = vapaee::dex::market::aux_get_canonical_market_id(token, to_token);
                                name converter1 = aux_get_any_conveter_for_market_id(market1);
                                name converter2 = aux_get_any_conveter_for_market_id(market2);
                                converter = converter1;
                                path = converter.to_string()+"/"+token.to_string()+" "+converter2.to_string()+"/"+to_token.to_string();
                                break;
                            }
                        }
                        itr++;
                    }
                }
                
                vapaee::pool::util::send_swap(
                    from,
                    quantity,
                    token,
                    recipiant,
                    swapmemo,
                    converter,
                    path
                );           

                PRINT("vapaee::dex::swap::handle_start_swap_transfer() ...\n");
            }

            // --------------------------------------------------------------

            
        };     
    };
};

#include <vapaee/pool/modules/util.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/dex/modules/market.hpp>