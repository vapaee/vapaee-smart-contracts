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
