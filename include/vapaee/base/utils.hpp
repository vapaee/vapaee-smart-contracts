#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/error.hpp>

#include <tuple>
#include <cmath>
#include <string>
#include <cctype>
#include <locale>
#include <cstring>
#include <algorithm>
#include <functional>

#include <stdlib.h>


using std::get;
using std::map;
using std::max;
using std::tuple;
using std::array;
using std::string;
using std::vector;
using std::strlen;
using std::strtok;
using std::strtoll;
using std::replace;
using std::to_string;
using std::make_tuple;

using eosio::check;
using eosio::asset;
using eosio::symbol;


namespace vapaee {
    namespace utils {      
        uint64_t round_amount(uint64_t amount) {
            // PRINT("vapaee::utils::round_amount()\n");
            // PRINT(" amount:   ", std::to_string((unsigned long long) amount), "\n");

            uint64_t diff = amount % 100;
            // PRINT(" -> diff:   ", std::to_string((unsigned long long) diff), "\n");
            if (diff <= 5)  { amount = amount - diff; }
            if (diff >= 95) { amount = amount + (100 - diff); }
            // PRINT(" -> FINAL: ", std::to_string((unsigned long long) amount), "\n");
            // PRINT("vapaee::utils::round_amount()...\n");
            return amount;
        }

        static inline checksum256 hash(std::string s) {
            return sha256(const_cast<char*>(s.c_str()), s.size());
        }

        static uint32_t prng_range(uint64_t nonce, uint32_t to) {
            uint64_t seed = current_time_point().time_since_epoch().count() + nonce;
            checksum256 hash = sha256((char *)&seed, sizeof(seed));
            uint32_t aux;
            memcpy(&aux, &hash, sizeof(aux));

            // seed = result.hash[1];
            // seed <<= 32;
            // seed |= result.hash[0];
            uint32_t result = (uint32_t)(aux % to);
            return result;
        }

        static string prng_token(uint32_t size) {
            const char* CHARSET = "abcdefghijklmnopqrstuvwxyz0123456789";
            const uint32_t CHARSET_SIZE = 36;
            string token(size, ' ');
            for (int i = 0; i < size; i++)
                token[i] = CHARSET[prng_range(i, CHARSET_SIZE)];
            return token;
        }

        vector<string> split(const string &txt, const char * delim) {
            vector<string> tokens;
            string buffer(txt);
            char* token = strtok(&buffer[0], delim);
            while(token) {
                tokens.push_back(string(token, token + strlen(token)));
                token = strtok(NULL, delim);
            }
            return tokens;
        }

        string join(vector<string> parts, string separator) {
            string ret;
            int i = 1;
            for(auto& part : parts) {
                ret += part;
                if (i++ < parts.size())
                    ret += separator;
            }
            return ret;
        }
    
        string to_lowercase(string str) {
            string result;
            for (auto &c : str)
                result += tolower(c);
            return result;
        }

        inline int64_t ipow(int64_t base, uint64_t exp) {
            if (exp == 0) return 1;

            int64_t result = base;
            for(int i = 0; i < (exp - 1); i++)
                result *= base;

            return result;
        }

        inline int abs(int n) {
            if (n < 0)
                return n * -1;
            else
                return n;
        }

        #define ERR_SPLIT_SIZE "incorrect size after split"

        asset asset_from_string(string asset_str) {
            vector<string> asset_tokens = split(asset_str, " ");
            check(asset_tokens.size() == 2, ERR_SPLIT_SIZE);

            string str_amount = asset_tokens[0];
            string str_symbol = asset_tokens[1];

            uint8_t asset_precision = str_amount.size() - str_amount.find('.') - 1;
            symbol sym(
                symbol_code(str_symbol), asset_precision 
            );

            str_amount.erase(
                remove(str_amount.begin(), str_amount.end(), '.'),
                str_amount.end()
            );

            int64_t amount = strtoll(str_amount.c_str(), nullptr, 10);
            
            return asset(amount, sym);
        }

        int128_t divide(const asset &A, const asset &B) {
            check(B.amount > 0, "can't divide by zero");
            check(A.symbol.precision() == B.symbol.precision(),
                    "same precision only for now");

            int128_t _a = A.amount;
            int128_t _b = B.amount;

            // perform operation and add extra precision destroyed by operation
            int128_t result = (
                _a * ipow(10, B.symbol.precision())
            ) / _b;
            
            return result;
        }

        asset asset_divide(const asset &A, const asset &B) {
            return asset(divide(A, B), A.symbol);
        }

        int128_t multiply(const asset &A, const asset &B) {
            asset accurate, inaccurate;

            if (A.symbol.precision() > B.symbol.precision()) {
                accurate = A;
                inaccurate = B;
            } else {
                accurate = B;
                inaccurate = A;
            }

            // augment precision of most inaccurate amount
            int dif = accurate.symbol.precision() - inaccurate.symbol.precision();

            int128_t _accurate_amount = accurate.amount;
            int128_t _fixed_amount = (int128_t)inaccurate.amount * ipow(10, dif);

            // perform operation and remove extra precision created by operation
            int128_t result = _fixed_amount * _accurate_amount;
            result /= ipow(10, accurate.symbol.precision());

            // if function should return amount using
            // most accurate precision ret here

            // if function should return using B asset precision 
            dif = accurate.symbol.precision() - B.symbol.precision();

            if (dif == 0) // B was the most accurate
                return result;
            else
                return result / ipow(10, dif);
                // dif > 0, B had less precision than A, remove extra precision


        }

        asset asset_multiply(const asset &A, const asset &B) {
            return asset(multiply(A, B), B.symbol);
        }

        double to_double(const asset &A) {
            double amount = (double)A.amount;
            double unit = (double)pow(10.0, A.symbol.precision());
            double result = amount / unit;            
            return result;
        }

        asset inverse(const asset &A, const symbol &B ) {
            int dif = A.symbol.precision() - B.precision();
            uint8_t prec = max(A.symbol.precision(), B.precision());

            int64_t amount = A.amount;
            if (dif < 0)
                amount *= ipow(10, -dif);

            int128_t A_inverse = ipow(10, prec * 2) / amount;

            if (dif > 0)
                A_inverse /= ipow(10, dif);

            return asset((int64_t)A_inverse, B);
        }

        asset asset_change_precision(const asset &A, uint8_t target) {
            asset changed = A;
            changed.symbol = symbol(A.symbol.code(), target);
            int dif = target - A.symbol.precision();
            if (dif > 0)
                changed.amount *= ipow(10, dif);
            else
                changed.amount /= ipow(10, -dif);
            
            return changed;
        }

        asset asset_change_symbol(const asset &A, const symbol &sym) {
            asset changed = A;
            changed.symbol = sym;
            int dif = sym.precision() - A.symbol.precision();
            if (dif > 0)
                changed.amount *= ipow(10, dif);
            else
                changed.amount /= ipow(10, -dif);
            
            return changed;

        }

        uint128_t pack(uint64_t a, uint64_t b) {
            return ((uint128_t)a << 64 ) | (uint128_t)b;
        }

        uint128_t symbols_get_index(symbol_code A, symbol_code B) {
            return pack(A.raw(), B.raw());
        }

        // -------------------------------------------------------------

        inline string create_error_string1(const char * text, const string str) {
            return string(text) + " [" + str + "]";
        }

        inline string create_error_string2(const char * text, const string str1, const string str2) {
            return string(text) + " [" + str1 + ", " + str2 + "]";
        }

        inline string create_error_string3(const char * text, const string str1, const string str2, const string str3) {
            return string(text) + " [" + str1 + ", " + str2 + ", " + str3 + "]";
        }

        inline string create_error_string4(const char * text, const string str1, const string str2, const string str3, const string str4) {
            return string(text) + " [" + str1 + ", " + str2 + ", " + str3 + ", " + str4 + "]";
        }

        inline string create_error_id1(const char * text, const uint64_t id) {
            return string(text) + " [" + std::to_string((unsigned long) id ) + "]";
        }

        inline string create_error_id2(const char * text, const uint64_t id1, const uint64_t id2) {
            return string(text) + " [" + std::to_string((unsigned long) id1 ) + ", " + std::to_string((unsigned long) id2 ) + "]";
        }

        inline string create_error_id3(const char * text, const uint64_t id1, const uint64_t id2, const uint64_t id3) {
            return string(text) + " [" + std::to_string((unsigned long) id1 ) + ", " + std::to_string((unsigned long) id2 ) + ", " + std::to_string((unsigned long) id3 ) + "]";
        }

        inline string create_error_double1(const char * text, const double value) {
            return string(text) + " [" + std::to_string( value ) + "]";
        }

        inline string create_error_double2(const char * text, const double value1, const double value2) {
            return string(text) + " [" + std::to_string( value1 ) + ", " + std::to_string( value2 ) + "]";
        }

        inline string create_error_double3(const char * text, const double value1, const double value2, const double value3) {
            return string(text) + " [" + std::to_string( value1 ) + ", " + std::to_string( value2 ) + ", " + std::to_string( value3 ) + "]";
        }

        inline string create_error_float1(const char * text, const float value) {
            return string(text) + " [" + std::to_string( value ) + "]";
        }

        inline string create_error_float2(const char * text, const float value1, const float value2) {
            return string(text) + " [" + std::to_string( value1 ) + ", " + std::to_string( value2 ) + "]";
        }

        inline string create_error_float3(const char * text, const float value1, const float value2, const float value3) {
            return string(text) + " [" + std::to_string( value1 ) + ", " + std::to_string( value2 ) + ", " + std::to_string( value3 ) + "]";
        }

        inline string create_error_symcode1(const char * text, const symbol_code & sym1) {
            return string(text) + " [" + sym1.to_string() + "]";
        }

        inline string create_error_symcode2(const char * text, const symbol_code & sym1, const symbol_code & sym2) {
            return string(text) + " [" + sym1.to_string() + "], [" + sym2.to_string()+"]";
        }

        inline string create_error_symbol1(const char * text, const symbol & sym1) {
            return string(text) + " [" + sym1.code().to_string() + "]";
        }

        inline string create_error_symbol2(const char * text, const symbol & sym1, const symbol & sym2) {
            return string(text) + " [" + sym1.code().to_string() + "], [" + sym2.code().to_string()+"]";
        }

        inline string create_error_name1(const char * text, const name & account1) {
            return string(text) + " [" + account1.to_string() + "]";
        }

        inline string create_error_name2(const char * text, const name & account1, const name & account2) {
            return string(text) + " [" + account1.to_string() + "," + account2.to_string() + "]";
        }

        inline string create_error_asset1(const char * text, const asset & token1) {
            return string(text) + " [" + token1.to_string() + "]";
        }

        inline string create_error_asset2(const char * text, const asset & token1, const asset & token) {
            return string(text) + " [" + token1.to_string() + "], [" + token.to_string()+"]";
        }

        inline string create_error_asset3(const char * text, const asset & token1, const asset & token, const asset & token3) {
            return string(text) + " [" + token1.to_string() + "], [" + token.to_string()+"], [" + token3.to_string()+"]";
        }

        inline string create_error_asset4(const char * text, const asset & token1, const asset & token, const asset & token3, const asset & token4) {
            return string(text) + " [" + token1.to_string() + "], [" + token.to_string()+"], [" + token3.to_string()+"], [" + token4.to_string()+"]";
        }

        inline string create_error_asset5(const char * text, const asset & token1, const asset & token, const asset & token3, const asset & token4, const asset & token5) {
            return string(text) + " [" + token1.to_string() + "], [" + token.to_string()+"], [" + token3.to_string()+"], [" + token4.to_string()+"], [" + token5.to_string()+"]";
        }

        inline string create_error_bool1(const char * text, const bool value) {
            return string(text) + " [" + std::to_string(value) + "]";
        }

        inline string create_error_bool2(const char * text, const bool value1, const bool value2) {
            return string(text) + " [" + std::to_string(value1) + "], [" + std::to_string(value2) + "]";
        }

        inline string create_error_bool3(const char * text, const bool value1, const bool value2, const bool value3) {
            return string(text) + " [" + std::to_string(value1) + "], [" + std::to_string(value2) + "], [" + std::to_string(value3) + "]";
        }

        inline string create_error_bool4(const char * text, const bool value1, const bool value2, const bool value3, const bool value4) {
            return string(text) + " [" + std::to_string(value1) + "], [" + std::to_string(value2) + "], [" + std::to_string(value3) + "], [" + std::to_string(value4) + "]";
        }

        // -------------------------------------------------------------

        uint8_t char_to_value( char c ) {
            if( c == '.')
                return 0;
            else if( c >= '1' && c <= '5' )
                return (c - '1') + 1;
            else if( c >= 'a' && c <= 'z' )
                return (c - 'a') + 6;
            else
                eosio::check( false, create_error_string1(ERROR_CTV_1, std::to_string(c)).c_str());

            return 0;
        }

        name check_name_from_string(string str) {
            uint64_t value = 0;
            if( str.size() > 13 ) {
                eosio::check( false, create_error_string1(ERROR_CNFS_1, str).c_str());
            }
            if( str.empty() ) {
                eosio::check( false, create_error_string1(ERROR_CNFS_2, str).c_str());
            }

            auto n = std::min( (uint32_t)str.size(), (uint32_t)12u );
            for( decltype(n) i = 0; i < n; ++i ) {
                value <<= 5;
                value |= char_to_value( str[i] );
            }
            value <<= ( 4 + 5*(12 - n) );
            if( str.size() == 13 ) {
                uint64_t v = char_to_value( str[12] );
                if( v > 0x0Full ) {
                    eosio::check(false, create_error_string1(ERROR_CNFS_3, str).c_str());
                }
                value |= v;
            }

            return name(value);
        }

        symbol_code check_symbol_code_from_string(string str) {
            uint64_t value = 0;
            if( str.size() > 7 ) {
                eosio::check( false, create_error_string1(ERROR_CSCFS_1, str).c_str());
            }
            for( auto itr = str.rbegin(); itr != str.rend(); ++itr ) {
                if( *itr < 'A' || *itr > 'Z') {
                    eosio::check( false, create_error_string1(ERROR_CSCFS_2, str).c_str());
                }
                value <<= 8;
                value |= *itr;
            }
            symbol_code code(str.c_str());
            return code;
        }

        float check_float_from_string(string str) {
            return std::stof(str, 0);
        }

        uint32_t check_integer_from_string(string str) {
            uint32_t value = std::atoi(str.c_str());
            // uint32_t nowsec = current_time_point().sec_since_epoch();
            // check(value <= nowsec, create_error_id1(ERROR_ACIFS_1, value).c_str());
            return value;
        }

        asset check_asset_from_string(string str) {
           
            int i = str.find(" ");

            eosio::check( i != -1, create_error_string1(ERROR_ACFS_1, str).c_str());
                        
            string param1 = str.substr(0, i);
            string param2 = str.substr(i + 1);
            
            symbol_code sym_code = check_symbol_code_from_string(param2);

            int dot_index = param1.find('.');
            uint8_t precision = param1.length() - (dot_index + 1);

            param1.erase(std::remove(param1.begin(), param1.end(), '.'), param1.end());

            uint64_t amount = std::atoi(param1.c_str()); 

            return asset(amount, symbol(sym_code, precision));
        }

        int min (int a, int b) {
            return a < b ? a : b;
        }
        int max (int a, int b) {
            return a > b ? a : b;
        }


    }; // namespace utils
}; // namespace vaapee
