#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/base.hpp>

namespace vapaee {
    namespace profile {
        namespace core {
            
            void action_test(name contract, const slug & alias, uint8_t precision) {
                PRINT("eosio::dex::token::action_add_token()\n");
                PRINT(" contract: ", contract.to_string(), "\n");
                PRINT(" alias: ", alias.to_string(), "\n");
                PRINT(" precision: ", std::to_string((unsigned) precision), "\n");

                check(1 > 0, "Yes, this works!");                

                PRINT("eosio::dex::token::action_add_token() ...\n");
            }

        };     
    };
};
