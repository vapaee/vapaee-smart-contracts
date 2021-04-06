#pragma once
#include <vapaee/base/base.hpp>

using namespace std;
using namespace eosio;

namespace vapaee {
    namespace dex {
        namespace error {
        
            // inline name contract { return vapaee::dex::contract; }
            // inline name get_code() { return vapaee::dex::contract; }

            inline string create_error_string1(const char * text, const string str) {
                return string(text) + " [" + str + "]";
            }

            inline string create_error_string2(const char * text, const string str1, const string str2) {
                return string(text) + " [" + str1 + ", " + str2 + "]";
            }

            inline string create_error_id1(const char * text, const uint64_t id) {
                return string(text) + " [" + std::to_string((unsigned long) id ) + "]";
            }

            inline string create_error_id2(const char * text, const uint64_t id1, const uint64_t id2) {
                return string(text) + " [" + std::to_string((unsigned long) id1 ) + ", " + std::to_string((unsigned long) id2 ) + "]";
            }            

            inline string create_error_double1(const char * text, const double value) {
                return string(text) + " [" + std::to_string( value ) + "]";
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
        }
    }
}