#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/token/modules/standard.hpp>
#include <vapaee/token/modules/wrapper.hpp>

namespace vapaee {
    namespace token {
        namespace issuance {

            inline name get_self() {
                return vapaee::token::contract;
            }

            void action_chissuer(const name& new_issuer, const symbol_code& token) {
                PRINT("vapaee::token::issuance::action_chissuer()\n");
                PRINT(" new_issuer: ", new_issuer.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");

                stats statstable( get_self(), token.raw() );
                auto ptr = statstable.find( token.raw() );
                check( ptr != statstable.end(), "[chissuer] token with token does not exist, create token before issue (vapaeetokens)" );

                require_auth(ptr->issuer);

                statstable.modify( *ptr, same_payer, [&]( auto& s ) {
                    s.issuer = new_issuer;
                });
                
            }
            
            void action_issuer(const name& action, const name& theissuer, const asset& max) {
                PRINT("vapaee::token::issuance::action_issuer()\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" theissuer: ", theissuer.to_string(), "\n");
                PRINT(" max: ", max.to_string(), "\n");

                symbol_code token = max.symbol.code();
                PRINT(" > token: ", token.to_string(), "\n");

                stats statstable( get_self(), token.raw() );
                auto ptr = statstable.find( token.raw() );
                check( ptr != statstable.end(), "[chissuer] token with token does not exist, create token before issue (vapaeetokens)" );

                require_auth(ptr->issuer);
                name ram_payer = ptr->issuer;

                issuers issuerstable( get_self(), token.raw() );
                auto iss_ptr = issuerstable.find( theissuer.value );

                if (action == name("add")) {
                    if (iss_ptr == issuerstable.end()) {
                        issuerstable.emplace(ram_payer, [&]( auto& a ){
                            a.issuer = theissuer;
                            a.max    = max;
                        });
                    } else {
                        issuerstable.modify(*iss_ptr, ram_payer, [&]( auto& a ){
                            a.max    += max;
                        });
                    }
                } else if (action == name("remove")) {
                    check(iss_ptr != issuerstable.end(), 
                        create_error_string2("ERR-AIS-01: can't remove issuer for token. It has not found. (the issuer, token) ",
                            theissuer.to_string(),
                            token.to_string()
                        ).c_str()
                    );
                }
            }

        };     
    };
};
