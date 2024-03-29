#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>

namespace vapaee {
    namespace dex {
        namespace swap {
            void handle_start_swap_transfer(name from, name to, asset quantity, string memo, name tokencontract);
        };
    };
};

namespace vapaee {
    namespace dex {

        namespace utils {

            name get_self() {
                return vapaee::dex::contract;
            }

            name get_contract_for_token(symbol_code sym) {
                vapaee::dex::tokens book_tokens(get_self(), get_self().value);
                auto book_it = book_tokens.find(sym.raw());
                check(book_it != book_tokens.end(), ERROR_UGCFT_1);
                return book_it->contract;
            }

            inline name aux_get_modify_payer(name owner) {
                if (owner == get_self()) return same_payer;
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
                // PRINT("vapaee::dex::utils::aux_extend_asset()\n");
                return asset_change_precision(
                    quantity, dex_internal_precision);
            }
            
            asset aux_get_real_asset(const asset & quantity_extended) {
                PRINT("vapaee::dex::utils::aux_get_real_asset()\n");
                PRINT("  quantity_extended: ", quantity_extended.to_string(), "\n");
                asset real = quantity_extended;
                uint64_t amount = quantity_extended.amount;
                uint8_t precision = quantity_extended.symbol.precision();
                symbol_code sym_code = quantity_extended.symbol.code();

                //check(vapaee::dex::internal_precision == precision, create_error_id1(ERROR_AGEA_1, precision).c_str());

                vapaee::dex::tokens tokenstable(get_self(), get_self().value);
                PRINT(" tokens tokenstable(get_self(), get_self().value);\n");
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
                    blacklist list(get_self(), get_self().value); 
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
                if (dex_internal_precision == precision) return quantity_extended;

                // extension
                int extension = dex_internal_precision - precision;
                amount /= ipow(10, extension);

                real.amount = amount;
                real.symbol = symbol(sym_code, precision);
                PRINT(" real asset: ",real.to_string(),"\n");
                PRINT("vapaee::dex::utils::aux_get_real_asset() ...\n");
                return real;
            }

            void send_swap(const asset& quantity, const symbol_code& token_to_receive, const name& recipiant, const string& memo) {
                PRINT("vapaee::dex::utils::send_swap()\n");

                // vapaee::token::utils::send_transfer_tokens(vapaee::current_contract, vapaee::dex::contract, quantity, swap_memo);
                name from = vapaee::current_contract;
                name to = vapaee::dex::contract;
                name token_contract = vapaee::dex::utils::get_contract_for_token(quantity.symbol.code());
                string swap_memo = string("openpool.v1;")+token_to_receive.to_string()+";"+recipiant.to_string()+";"+memo ;
                
                vapaee::dex::swap::handle_start_swap_transfer(from, to, quantity, swap_memo, token_contract );
            }
        };       
    };
};

#include <vapaee/dex/modules/swap.hpp>