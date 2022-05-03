#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>

using namespace std;
#define TOKEN_GROUP_ZERO 0

namespace vapaee {
    namespace dex {
        using namespace utils;
        
        namespace security {

            bool aux_is_token_blacklisted(const symbol_code &sym_code, name tokencontract) {
                // PRINT("vapaee::dex::security::aux_is_token_blacklisted()\n");
                // PRINT(" sym_code: ", sym_code.to_string(), "\n");
                // PRINT(" tokencontract: ", tokencontract.to_string(), "\n");

                blacklist list(contract, contract.value); 
                auto index = list.get_index<name("symbol")>();
                auto itr = index.lower_bound(sym_code.raw());
                for (itr = index.lower_bound(sym_code.raw()); itr != index.end(); itr++) {
                    if (itr->symbol == sym_code) {
                        if (itr->contract == tokencontract) {
                            // PRINT("vapaee::dex::security::aux_is_token_blacklisted() ...\n");
                            return true;
                        }
                    } else {
                        break;
                    }
                }
                // PRINT("vapaee::dex::security::aux_is_token_blacklisted() ...\n");
                return false;
            }

            bool aux_is_token_blacklisted(const symbol_code &sym_code) {
                // PRINT("vapaee::dex::security::aux_is_token_blacklisted()\n");
                // PRINT(" sym_code: ", sym_code.to_string(), "\n");
                tokens token_table(contract, contract.value);
                auto ptr = token_table.find(sym_code.raw());
                check(ptr != token_table.end(), create_error_symcode1(ERROR_AITB_1, sym_code).c_str());
                bool result = aux_is_token_blacklisted(sym_code, ptr->contract);
                // PRINT("vapaee::dex::security::aux_is_token_blacklisted() ...\n");
                return result;
            }

            // -------------------------------------------------------------
            
            bool aux_is_token_registered(symbol& sym) {
                tokens tokenstable(contract, contract.value);
                auto ptr = tokenstable.find(sym.code().raw());
                return ptr != tokenstable.end();
            }

            bool aux_is_token_tradeable(symbol& sym) {
                tokens tokenstable(contract, contract.value);
                auto ptr = tokenstable.find(sym.code().raw());
                if (ptr != tokenstable.end()) {
                    return ptr->tradeable;
                }
                return false;
            }
            
            bool aux_is_token_genuine(symbol& sym, name tokencontract) {
                tokens tokenstable(contract, contract.value);
                auto ptr = tokenstable.find(sym.code().raw());
                if (ptr != tokenstable.end()) {
                    return ptr->contract == tokencontract;
                }
                return false;
            }

            bool aux_is_token_blacklisted(const symbol& sym) {
                tokens tokenstable(contract, contract.value);
                auto ptr = tokenstable.find(sym.code().raw());
                if (ptr != tokenstable.end()) {
                    aux_is_token_blacklisted(ptr->symbol, ptr->contract);
                }
                return false;
            }

            // -------------------------------------------------------------

            string combine_error_code(string error_code, string error_message) {
                return error_code + error_message;
            }

            void aux_check_token_ok(const symbol& sym, name tokencontract, string error_code) {
                tokens tokenstable(contract, contract.value);
                auto ptr = tokenstable.find(sym.code().raw());

                // is token registered
                if (ptr == tokenstable.end()) {
                    check(false, create_error_symbol1(combine_error_code(error_code, ERROR_ACTOK_1).c_str(), sym).c_str());
                }
    
                // is token genuine
                if (ptr->contract != tokencontract) {
                    check(false, create_error_name2(combine_error_code(error_code, ERROR_ACTOK_2).c_str(), ptr->contract, tokencontract).c_str());
                }

                // is token tradeable
                if (!ptr->tradeable) {
                    check(false, create_error_symbol1(combine_error_code(error_code, ERROR_ACTOK_3).c_str(), sym).c_str());
                }

                // is token blacklisted
                if (aux_is_token_blacklisted(sym)) {
                    check(false, create_error_symbol1(combine_error_code(error_code, ERROR_ACTOK_4).c_str(), sym).c_str());
                }
            }

            bool aux_is_token_ok(symbol& sym, name tokencontract) {
                tokens tokenstable(contract, contract.value);
                auto ptr = tokenstable.find(sym.code().raw());

                // is token registered
                if (ptr == tokenstable.end()) {
                    return false;
                }

                // is token genuine
                if (ptr->contract == tokencontract) {
                    return false;
                }

                // is token tradeable
                if (!ptr->tradeable) {
                    return false;
                }

                // is token blacklisted
                if (aux_is_token_blacklisted(sym)) {
                    return false;
                }
               
                return true;
            }

        };     
    };
};
