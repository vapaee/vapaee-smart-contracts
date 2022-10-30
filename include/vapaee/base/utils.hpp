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
// #include <regex>

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

#define TYPERR_NO_ERROR  0
#define TYPERR_SYMCODE_1 1
#define TYPERR_SYMCODE_2 2
#define TYPERR_NAME_1    3
#define TYPERR_NAME_2    4 
#define TYPERR_NAME_3    5
#define TYPERR_NAME_4    6
#define TYPERR_ASSET_1   7


namespace vapaee {
    namespace utils {      

        static inline checksum256 hash(std::string s) {
            // vapaee::utils::hash("hello");
            return sha256(const_cast<char*>(s.c_str()), s.size());
        }

        vector<string> split(const string &txt, const char delim) {
            // vapaee::utils::split("hello,world", ',');
            vector<string> tokens;
            int s=0;
            for (int i=0; i<txt.size(); i++) {
                if (txt[i] == delim) {
                    tokens.push_back(txt.substr(s, i-s));
                    s = i+1;
                }
            }
            if (s <= txt.length()) {
                tokens.push_back(txt.substr(s));
            }
            
            return tokens;
        }

        vector<string> split(const string &txt, const char * delim) {
            return split(txt, *delim);
        }

        string join(vector<string> parts, string separator) {
            // vapaee::utils::join({"hello", "world"}, ",");
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
            // vapaee::utils::to_lowercase("HELLO");
            string result;
            for (auto &c : str)
                result += tolower(c);
            return result;
        }

        inline int64_t ipow(int64_t base, uint64_t exp) {
            // vapaee::utils::ipow(2, 3);
            if (exp == 0) return 1;

            int64_t result = base;
            for(int i = 0; i < (exp - 1); i++)
                result *= base;

            return result;
        }

        inline int abs(int n) {
            // vapaee::utils::abs(-1);
            if (n < 0)
                return n * -1;
            else
                return n;
        }

        int128_t divide(const asset &A, const asset &B) {
            // vapaee::utils::divide(asset(1, symbol("TLOS", 4)), asset(1, symbol("TLOS", 4)));
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
            // vapaee::utils::asset_divide(asset(1, symbol("TLOS", 4)), asset(1, symbol("TLOS", 4)));
            return asset(divide(A, B), A.symbol);
        }

        int128_t multiply(const asset &A, const asset &B) {
            // vapaee::utils::multiply(asset(1, symbol("TLOS", 4)), asset(1, symbol("TLOS", 4)));
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
            // vapaee::utils::asset_multiply(asset(1, symbol("TLOS", 4)), asset(1, symbol("TLOS", 4)));
            return asset(multiply(A, B), B.symbol);
        }

        double asset_to_double(const asset &A) {
            // vapaee::utils::asset_to_double(asset(1, symbol("TLOS", 4)));
            double amount = (double)A.amount;
            double unit = (double)pow(10.0, A.symbol.precision());
            double result = amount / unit;            
            return result;
        }

        asset inverse(const asset &A, const symbol &B ) {
            // vapaee::utils::inverse(asset(1, symbol("TLOS", 4)), symbol("TLOS", 4));
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
            // vapaee::utils::asset_change_precision(asset(1, symbol("TLOS", 4)), 6);
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
            // vapaee::utils::asset_change_symbol(asset(1, symbol("TLOS", 4)), symbol("TLOS", 4));
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
            // vapaee::utils::pack( token.raw(), poollabel.value);
            return ((uint128_t)a << 64 ) | (uint128_t)b;
        }

        uint128_t pack_symbols_in_uint128(symbol_code A, symbol_code B) {
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

        inline string time_point_sec_to_string(const time_point_sec value) {
            // I didn't find a simple way to solve this
            return std::to_string((long)value.sec_since_epoch());
        }

        inline string create_error_time1(const char * text, const time_point_sec value1) {
            return string(text) + " [" + time_point_sec_to_string(value1) + "]";
        }

        inline string create_error_time2(const char * text, const time_point_sec value1, const time_point_sec value2) {
            return string(text) + " [" + time_point_sec_to_string(value1) + "], [" + time_point_sec_to_string(value2) + "]";
        }

        inline string create_error_time3(const char * text, const time_point_sec value1, const time_point_sec value2, const time_point_sec value3) {
            return string(text) + " [" + time_point_sec_to_string(value1) + "], [" + time_point_sec_to_string(value2) + "], [" + time_point_sec_to_string(value3) + "]";
        }

        inline string create_error_time4(const char * text, const time_point_sec value1, const time_point_sec value2, const time_point_sec value3, const time_point_sec value4) {
            return string(text) + " [" + time_point_sec_to_string(value1) + "], [" + time_point_sec_to_string(value2) + "], [" + time_point_sec_to_string(value3) + "], [" + time_point_sec_to_string(value4) + "]";
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
                eosio::check( false, create_error_string1(ERROR_CTV_1, string("'") + c + "'").c_str());

            return 0;
        }

        bool is_char_valid_for_name( char c ) {
            if( c == '.')
                return true;
            else if( c >= '1' && c <= '5' )
                return true;
            else if( c >= 'a' && c <= 'z' )
                return true;
            else
                return false;
            return false;
        }

        int get_name_from_string(const string str, name &result) {
            
            uint64_t value = 0;
            if( str.size() > 13 ) {
                PRINT(create_error_string1(ERROR_CNFS_1, str).c_str(),"\n");
                return TYPERR_NAME_1;
            }
            if( str.empty() ) {
                PRINT(create_error_string1(ERROR_CNFS_2, str).c_str(),"\n");
                return TYPERR_NAME_2;
            }

            auto n = std::min( (uint32_t)str.size(), (uint32_t)12u );
            for( decltype(n) i = 0; i < n; ++i ) {
                if (!is_char_valid_for_name( str[i] )) {
                    return TYPERR_NAME_3;
                }
            }
            for( decltype(n) i = 0; i < n; ++i ) {
                value <<= 5;
                value |= char_to_value( str[i] );
            }
            value <<= ( 4 + 5*(12 - n) );
            if( str.size() == 13 ) {
                if (!is_char_valid_for_name( str[12] )) {
                    return TYPERR_NAME_3;
                }
                uint64_t v = char_to_value( str[12] );
                if( v > 0x0Full ) {
                    PRINT(create_error_string1(ERROR_CNFS_4, str).c_str(),"\n");
                    return TYPERR_NAME_4;
                }
                value |= v;
            }

            result = name(value);
            return TYPERR_NO_ERROR;

        }

        name check_name_from_string(string str) {
            name result;
            int fail = get_name_from_string(str, result);
            switch (fail) {
                case TYPERR_NAME_1: eosio::check( false, create_error_string1(ERROR_CNFS_1, str).c_str());
                case TYPERR_NAME_2: eosio::check( false, create_error_string1(ERROR_CNFS_2, str).c_str());
                case TYPERR_NAME_3: eosio::check( false, create_error_string1(ERROR_CNFS_3, str).c_str());
                case TYPERR_NAME_4: eosio::check( false, create_error_string1(ERROR_CNFS_4, str).c_str());
            }
            return result;
        }

        int get_symbol_code_from_string(const string str, symbol_code &result) {
            PRINT("vapaee::utils::get_symbol_code_from_string(", str.c_str(), ")\n");
            uint64_t value = 0;
            if( str.size() > 7 ) {
                return TYPERR_SYMCODE_1;
            }
            for( auto itr = str.rbegin(); itr != str.rend(); ++itr ) {
                if( *itr < 'A' || *itr > 'Z') {
                    return TYPERR_SYMCODE_2;
                }
                value <<= 8;
                value |= *itr;
            }
            symbol_code code(str.c_str());
            result = code;
            return TYPERR_NO_ERROR;
        }

        symbol_code check_symbol_code_from_string(string str) {
            symbol_code result;
            int fail = get_symbol_code_from_string(str, result);
            switch (fail) {
                case TYPERR_SYMCODE_1: eosio::check( false, create_error_string1(ERROR_CSCFS_1, str).c_str());
                case TYPERR_SYMCODE_2: eosio::check( false, create_error_string1(ERROR_CSCFS_2, str).c_str());
            }
            return result;            
        }

        float check_float_from_string(string str) {
            return std::stof(str, 0);
        }

        bool is_natural(const std::string &token) {
            // return std::regex_match(token, std::regex("[1-9][0-9]*$"));
            for (int i=0; i<token.size(); i++) {
                if (token[i] < '0' || '9' < token[i]) return false;
            }
            return token.size() > 0;
        }

        bool is_integer(const std::string &token) {
            string aux;
            if (token[0] == '+' || token[0] == '-') {
                aux = token.substr(1);
            } else {
                aux = token.substr(0);
            }
            return is_natural(aux);
        }

        bool isNumber(const std::string &token) {
            vector<string> parts = split(token, '.');
            switch(parts.size()) {
                case 1: {
                    return is_integer(parts[0]);
                }
                case 2: {
                    return is_integer(parts[0]) && is_natural(parts[1]);
                }
            }

            return false;
        }

        uint32_t check_integer_from_string(string str) {
            uint32_t value = std::atoi(str.c_str());
            return value;
        }

        int get_asset_from_string(string str, asset& result) {
            PRINT("vapaee::utils::get_asset_from_string(", str.c_str(), ")\n");

            int i = str.find(" ");

            if ( i == -1) return TYPERR_ASSET_1;
                        
            string param1 = str.substr(0, i);
            string param2 = str.substr(i + 1);
            
            symbol_code sym_code;
            int error = get_symbol_code_from_string(param2, sym_code);
            if (error) return error;

            int dot_index = param1.find('.');
            uint8_t precision = param1.length() - (dot_index + 1);

            param1.erase(std::remove(param1.begin(), param1.end(), '.'), param1.end());

            uint64_t amount = std::atoi(param1.c_str()); 

            result = asset(amount, symbol(sym_code, precision));
            return TYPERR_NO_ERROR;
        }

        asset check_asset_from_string(string str) {
            PRINT("vapaee::utils::check_asset_from_string(", str.c_str(), ")\n");
            asset result;
            int fail = get_asset_from_string(str, result);
            switch (fail) {
                case TYPERR_ASSET_1: eosio::check( false, create_error_string1(ERROR_ACFS_1, str).c_str());
                case TYPERR_SYMCODE_1: eosio::check( false, create_error_string1(ERROR_CSCFS_1, str).c_str());
                case TYPERR_SYMCODE_2: eosio::check( false, create_error_string1(ERROR_CSCFS_2, str).c_str());
            }
            return result;
        }

        int min (int a, int b) {
            return a < b ? a : b;
        }
        int max (int a, int b) {
            return a > b ? a : b;
        }

        template<typename CharT>
        std::string to_hex(const CharT* d, uint32_t s) {
            // vapaee::utils::to_hex(&checksum256_hash, sizeof(checksum256_hash));
            std::string r;
            const char* to_hex="0123456789abcdef";
            uint8_t* c = (uint8_t*)d;
            for( uint32_t i = 0; i < s; ++i ) {
                (r += to_hex[(c[i] >> 4)]) += to_hex[(c[i] & 0x0f)];
            }
            return r;
        }
    }; // namespace utils
}; // namespace vaapee
