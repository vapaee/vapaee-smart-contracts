#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/dex/modules/security.hpp>

namespace vapaee {
    namespace token {
        namespace issuance {

            inline name get_self() {
                return vapaee::token::contract;
            }

            void action_chissuer(const name& new_issuer, const symbol_code& symcode) {
                PRINT("vapaee::token::issuance::action_chissuer()\n");
                PRINT(" new_issuer: ", new_issuer.to_string(), "\n");
                PRINT(" symcode: ", symcode.to_string(), "\n");

                stats statstable( get_self(), symcode.raw() );
                auto ptr = statstable.find( symcode.raw() );
                check( ptr != statstable.end(), "[chissuer] token with symcode does not exist, create token before issue (vapaeetokens)" );

                require_auth(ptr->issuer);

                statstable.modify( *ptr, same_payer, [&]( auto& s ) {
                    s.issuer = new_issuer;
                });
                
                PRINT("vapaee::token::issuance::action_chissuer()...\n");
            }

        };     
    };
};
