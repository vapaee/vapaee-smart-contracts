#pragma once
#include <vapaee/base/base.hpp>

using namespace std;
using namespace eosio;

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cmath>



namespace vapaee {
    namespace utils {
        static uint128_t combine( uint64_t key1, uint64_t key2 ) {
            return (uint128_t{key1} << 64) | uint128_t{key2};
        }

        static uint128_t combine( uint64_t key1, name key2 ) {
            // return (uint128_t{key1.value} << 64) | uint128_t{key2};
            return vapaee::utils::combine(key1, key2.value);
        }

        static uint128_t combine( name key1, name key2 ) {
            return vapaee::utils::combine(key1.value, key2.value);
        }
        
        static asset inverse(const asset &A, const symbol &B ) {
                double A_amount = (double)A.amount;                        // 200000000
                double A_unit = (double)pow(10.0, A.symbol.precision());   // 100000000 
                double B_unit = (double)pow(10.0, B.precision());          //     10000 
                double A_real = A_unit / A_amount;                         //       0.5 
                int64_t amount = (int64_t) (A_real * B_unit);              //      5000
                asset inv = asset(amount, B);                              // 0.5000 TLOS
                return inv;
            return inv;
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

        static asset payment(const asset &total, const asset &price ) {
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
        static inline std::string &ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);} ));
            return s;
        }

        // trim from end
        static inline std::string &rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}  ).base(), s.end());
            return s;
        }

        // trim from both ends
        static inline std::string &trim(std::string &s) {
            return ltrim(rtrim(s));
        }

        static inline int64_t str_to_int64(const std::string &s) {
            return std::stoi(s);
        }
        
        static asset string_to_asset(const std::string& str) {
            // PRINT("vapaee::utils::string_to_asset()\n");
            // PRINT(" str: ", str.c_str(), "\n");
        
            string s = str;
            s = vapaee::utils::trim(s);

            // Find space in order to split amount and symbol
            auto space_pos = s.find(' ');
            check(space_pos != string::npos, "Asset's amount and symbol should be separated with space");
            std::string substring = s.substr(space_pos + 1);
            std::string symbol_str = vapaee::utils::trim(substring);
            std::string amount_str = s.substr(0, space_pos);

            // Ensure that if decimal point is used (.), decimal fraction is specified
            auto dot_pos = amount_str.find('.');
            if (dot_pos != string::npos) {
                check(dot_pos != amount_str.size() - 1, "Missing decimal fraction after decimal point");
            }

            // Parse symbol
            string precision_digit_str;
            uint8_t precision_digit = 0;
            if (dot_pos != string::npos) {
                precision_digit = amount_str.size() - dot_pos - 1;
            }

            // string symbol_part = precision_digit_str + ',' + symbol_str;
            symbol sym = symbol(symbol_code(symbol_str), precision_digit);

            // Parse amount
            int64_t int_part, fract_part, amount;
            if (dot_pos != string::npos) {
                // PRINT(" amount_str.substr(0, dot_pos): ", amount_str.substr(0, dot_pos).c_str(), "\n");
                // PRINT(" amount_str.substr(dot_pos + 1): ", amount_str.substr(dot_pos + 1).c_str(), "\n");
                int_part = vapaee::utils::str_to_int64(amount_str.substr(0, dot_pos));
                fract_part = vapaee::utils::str_to_int64(amount_str.substr(dot_pos + 1));
                if (amount_str[0] == '-') {
                    fract_part *= -1;
                }
                // PRINT(" int_part: ", std::to_string((long long) int_part), "\n");
                // PRINT(" fract_part: ", std::to_string((long long) fract_part), "\n");
            } else {
                int_part = vapaee::utils::str_to_int64(amount_str);
            }

            int_part = int_part * pow(10, sym.precision());
            
            amount = int_part + fract_part;
            // PRINT(" amount: ", std::to_string((long long) amount), "\n");
            // PRINT("vapaee::utils::string_to_asset() ....\n");
            return asset(amount, sym);
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
    }; // namespace utils
}; // namespace vaapee
