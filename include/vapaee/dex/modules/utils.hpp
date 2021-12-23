#pragma once
#include <vapaee/base/base.hpp>
// #include <vapaee/dex/modules/error.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cmath>

using namespace std;
using namespace eosio;

using std::max;

namespace vapaee {
    namespace dex {

        namespace utils {

            name get_contract_for_token(symbol_code sym) {
                tokens book_tokens(contract, contract.value);
                auto book_it = book_tokens.find(sym.raw());
                check(book_it != book_tokens.end(), ERROR_UGCFT_1);
                return book_it->contract;
            }

            inline name aux_get_modify_payer(name owner) {
                if (owner == vapaee::dex::contract) return same_payer;
                return (has_auth(owner)) ? owner : same_payer; 
            }

            /*
             *  auxiliary extend asset
             *
             *  converts assets with a precision lower than dex::internal_precision
             *  to that precision, correctly scales the amount
             *
             */
            asset aux_extend_asset(const asset & quantity) {
                return asset_change_precision(
                    quantity, vapaee::dex::internal_precision);
            }
            
            asset aux_get_real_asset(const asset & quantity_extended) {
                PRINT("vapaee::dex::utils::aux_get_real_asset()\n");
                PRINT("  quantity_extended: ", quantity_extended.to_string(), "\n");
                asset real = quantity_extended;
                uint64_t amount = quantity_extended.amount;
                uint8_t precision = quantity_extended.symbol.precision();
                symbol_code sym_code = quantity_extended.symbol.code();

                //check(vapaee::dex::internal_precision == precision, create_error_id1(ERROR_AGEA_1, precision).c_str());

                tokens tokenstable(contract, contract.value);
                PRINT(" tokens tokenstable(contract, contract.value);\n");
                auto tk_itr = tokenstable.find(quantity_extended.symbol.code().raw());
                PRINT(" auto tk_itr = tokenstable.find(quantity_extended.symbol.code().raw());\n");

                bool found = false;
                if (tk_itr != tokenstable.end()) {
                    found = true;
                    precision = tk_itr->precision;
                    PRINT(" tk_itr != tokenstable.end()  ---> precision: ",std::to_string((int) precision),"\n");
                } else {
                    // try and see if the token is_blacklisted
                    PRINT(" tk_itr == tokenstable.end()\n");
                    blacklist list(contract, contract.value); 
                    auto index = list.get_index<name("symbol")>();                    
                    for (auto itr = index.lower_bound(quantity_extended.symbol.code().raw()); itr != index.end(); itr++) {
                        if (itr->symbol == quantity_extended.symbol.code()) {
                            precision = itr->precision;
                            found = true;
                            PRINT(" tk_itr == tokenstable.end()  ---> precision: ",std::to_string((int) precision),"\n");
                            break;
                        }
                    }
                }

                check(found, create_error_symcode1(ERROR_AGEA_2, sym_code).c_str());

                // no extension
                if (vapaee::dex::internal_precision == precision) return quantity_extended;

                // extension
                int extension = vapaee::dex::internal_precision - precision;
                amount /= ipow(10, extension);

                real.amount = amount;
                real.symbol = symbol(sym_code, precision);
                PRINT(" real asset: ",real.to_string(),"\n");
                PRINT("vapaee::dex::utils::aux_get_real_asset() ...\n");
                return real;
            }
            
            // taken from here
            // https://github.com/EOSIO/eosio.cdt/issues/404#issuecomment-461626338
            float stof(std::string s, float def) {
                if (s == "") return def;
                std::size_t i = s.find(".");
                int digits = s.length() - i - 1;
                s.erase(i, 1); 
                float result = (float) ( atoi(s.c_str()) / pow(10, digits) );
                return result;
            }  

        };       
    };
};
