#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/modules/error.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cmath>

using namespace std;
using namespace eosio;


namespace vapaee {
    namespace dex {

        using namespace error;

        namespace utils {
        
            string to_lowercase(string str) {
                string result = str;
                for (int i=0; i<str.length(); i++) {
                    result[i] = tolower(str[i]);
                }
                return result;
            }
            
            inline uint128_t combine( uint64_t key1, uint64_t key2 ) {
                return (uint128_t{key1} << 64) | uint128_t{key2};
            }

            inline uint128_t combine( uint64_t key1, name key2 ) {
                // return (uint128_t{key1.value} << 64) | uint128_t{key2};
                return vapaee::dex::utils::combine(key1, key2.value);
            }

            inline uint128_t combine( name key1, name key2 ) {
                return vapaee::dex::utils::combine(key1.value, key2.value);
            }

            uint64_t round_amount(uint64_t amount) {
                // PRINT("vapaee::dex::utils::round_amount()\n");
                // PRINT(" amount:   ", std::to_string((unsigned long long) amount), "\n");

                uint64_t diff = amount % 100;
                // PRINT(" -> diff:   ", std::to_string((unsigned long long) diff), "\n");
                if (diff <= 5)  { amount = amount - diff; }
                if (diff >= 95) { amount = amount + (100 - diff); }
                // PRINT(" -> FINAL: ", std::to_string((unsigned long long) amount), "\n");
                // PRINT("vapaee::dex::utils::round_amount()...\n");
                return amount;
            }

            int64_t ipow(int64_t base, uint64_t exp) {
                if (exp == 0) return 1;

                int64_t result = base;
                for(int i = 0; i < (exp - 1); i++)
                    result *= base;

                return result;
            }

            int128_t multiply(const asset &A, const asset &B) {
                // PRINT("vapaee::dex::utils::multiply()\n");
                asset accurate, inaccurate;

                // PRINT("A precision: ", A.symbol.precision(), "\n");
                // PRINT("B precision: ", B.symbol.precision(), "\n");

                if (A.symbol.precision() > B.symbol.precision()) {
                    accurate = A;
                    inaccurate = B;
                    // PRINT("A is accurate\n");
                } else {
                    accurate = B;
                    inaccurate = A;
                    // PRINT("B is accurate\n");
                }

                // augment precision of most inaccurate amount
                int dif = accurate.symbol.precision() - inaccurate.symbol.precision();

                int128_t _accurate_amount = accurate.amount;
                int128_t _fixed_amount = (int128_t)inaccurate.amount * ipow(10, dif);

                // perform operation and remove extra precision created by operation
                int128_t result = _fixed_amount * _accurate_amount;
                result /= ipow(10, accurate.symbol.precision());

                // print("result amount: ");
                // print(result);
                // print("\n");

                // if function should return amount using
                // most accurate precision ret here

                // if function should return using B asset precision 
                dif = accurate.symbol.precision() - B.symbol.precision();

                // PRINT("vapaee::dex::utils::multiply()...\n");

                if (dif == 0) // B was the most accurate
                    return result;
                else { // dif > 0, B had less precision than A
                    // remove extra precision
                    for(int i = 0; i < dif; i++)
                        result /= 10;
                    return result;
                }

            }

            double to_double(const asset &A) {
                double amount = (double)A.amount;
                double unit = (double)pow(10.0, A.symbol.precision());
                double result = amount / unit;            
                return result;
            }

            asset inverse(const asset &A, const symbol &B ) {
                double A_amount = (double)A.amount;                        // 200000000
                double A_unit = (double)pow(10.0, A.symbol.precision());   // 100000000 
                double B_unit = (double)pow(10.0, B.precision());          //     10000 
                double A_real = A_unit / A_amount;                         //       0.5 
                int64_t amount = (int64_t) (A_real * B_unit);              //      5000
                asset inv = asset(amount, B);                              // 0.5000 TLOS
                return inv;
            }

            asset amount(const asset &price, const asset &payment, const symbol &B ) {
                // PRINT("vapaee::dex::utils::amount()\n");
                double price_amount = (double)price.amount;
                double unit = (double)pow(10.0, price.symbol.precision());
                double real_inverse = unit / price_amount;
                double real_amount = (double) (payment.amount * real_inverse);
                int64_t amount = (int64_t) (real_amount);
                int64_t fixed = round_amount(amount);
                if (amount != fixed) {
                    print("  amount: ", std::to_string((long long)amount), "\n");
                    print("  fixed: ", std::to_string((long long)fixed), "\n");
                }
                asset result = asset(amount, B);
                // PRINT("vapaee::dex::utils::amount()\n");
                return result;
            }

            asset payment(const asset &total, const asset &price ) {
                // PRINT("vapaee::dex::utils::payment()\n");
                // PRINT(" total: ", total.to_string(), "\n");
                // PRINT(" price: ", price.to_string(), "\n");
                double T_amount = (double)total.amount;
                double unit = (double)pow(10.0, total.symbol.precision());
                double T_real = T_amount / unit;
                int64_t amount = (int64_t) (T_real * price.amount);
                amount = round_amount(amount);
                asset pay = asset(amount, price.symbol);
                // PRINT("  pay: ", pay.to_string(), "\n");
                // PRINT("vapaee::dex::utils::payment() ...\n");
                return pay;
            }        


            // trim from start
            inline std::string &ltrim(std::string &s) {
                s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);} ));
                return s;
            }

            // trim from end
            inline std::string &rtrim(std::string &s) {
                s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}  ).base(), s.end());
                return s;
            }

            // trim from both ends
            inline std::string &trim(std::string &s) {
                return ltrim(rtrim(s));
            }

            inline int64_t str_to_int64(const std::string &s) {
                return std::stoi(s);
            }

            inline name aux_get_modify_payer(name owner) {
                if (owner == vapaee::dex::contract) return same_payer;
                return (has_auth(owner)) ? owner : same_payer; 
            }

            name aux_get_scope_for_tokens(const symbol_code & a, const symbol_code & b) {
                string a_sym_str = a.to_string();
                string b_sym_str = b.to_string();
                string scope_str = a_sym_str + "." + b_sym_str;
                scope_str = to_lowercase(scope_str);
                if (scope_str.size() > 12) {
                    scope_str = scope_str.substr(0,12);
                }
                name scope(scope_str);
                return scope;
            }

            /*
             *  auxiliary extend asset
             *
             *  converts assets with a precision lower than dex::internal_precision
             *  to that precision, correctly scales the amount
             *
             */
            asset aux_extend_asset(const asset & quantity) {
                PRINT("vapaee::dex::utils::aux_extend_asset()\n");
                asset extended = quantity;
                uint64_t amount = quantity.amount;
                uint8_t precision = quantity.symbol.precision();
                symbol_code sym_code = quantity.symbol.code();
                
                // no extension
                if (vapaee::dex::internal_precision == precision) return quantity;

                // extension
                uint8_t extension = vapaee::dex::internal_precision - precision;
                uint64_t multiplier = pow(10, extension);
                amount = amount * multiplier;

                extended.amount = amount;
                extended.symbol = symbol(sym_code, vapaee::dex::internal_precision);
                PRINT("vapaee::dex::utils::aux_extend_asset() ...\n");
                return extended;
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
                uint8_t extension = vapaee::dex::internal_precision - precision;
                uint64_t divider = pow(10, extension);
                amount = (uint64_t) ((double)amount / (double)divider);

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
