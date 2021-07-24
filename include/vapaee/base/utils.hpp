#pragma once
#include <vapaee/base/base.hpp>

#include <tuple>
#include <cmath>
#include <string>
#include <cctype>
#include <locale>
#include <cstring>
#include <algorithm>
#include <functional>

using std::max;
using std::tuple;
using std::array;
using std::string;
using std::vector;
using std::strlen;
using std::strtok;
using std::replace;

using eosio::check;
using eosio::asset;
using eosio::symbol;


namespace vapaee {
    namespace utils {      
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

        // // trim from start
        // static inline std::string &ltrim(std::string &s) {
        //     s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);} ));
        //     return s;
        // }

        // // trim from end
        // static inline std::string &rtrim(std::string &s) {
        //     s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}  ).base(), s.end());
        //     return s;
        // }

        // // trim from both ends
        // static inline std::string &trim(std::string &s) {
        //     return ltrim(rtrim(s));
        // }

        // static inline int64_t str_to_int64(const std::string &s) {
        //     return std::stoi(s);
        // }

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

        int128_t divide(const asset &A, const asset &B) {
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
            asset extended = A;
            extended.symbol = symbol(A.symbol.code(), target);
            int dif = target - A.symbol.precision();
            if (dif > 0)
                extended.amount *= ipow(10, dif);
            else
                extended.amount /= ipow(10, -dif);
            
            return extended;
        }
    }; // namespace utils
}; // namespace vaapee
