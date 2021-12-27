#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/book/modules/deposit.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/security.hpp>


namespace vapaee {
    namespace dex {
        namespace dao {
            asset aux_get_telos_decide_ballot_fee();
        }
    }
}

namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace fees {

            const name concept_ballot = name("ballot");
            const name concept_addtoken = name("addtoken");

            // payments -----------------------------------------------------
            void aux_delete_fees(name concept, name owner) {
                payments paytable(contract, concept.value);
                auto itr = paytable.find(owner.value);
                check(itr != paytable.end(),
                            create_error_name1(ERROR_ADBF_1, owner).c_str());
                paytable.erase(itr);
            }

            void aux_add_fees(name concept, name owner, asset quantity, name ram_payer) {
                payments paytable(contract, concept.value);
                auto itr = paytable.find(owner.value);
                check(itr == paytable.end(), create_error_name2(ERROR_AABF_1, concept, owner).c_str());
                paytable.emplace( ram_payer, [&]( auto& a ){
                    a.payer = owner;
                });
            }

            void handle_dex_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::dex::fees::handle_dex_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");                
                
                name the_concept;
                name sender  = from;
                check(is_account(sender), create_error_name1(ERROR_HDT_1, sender).c_str());

                if(memo == "ballot") {
                    // paying ballot fee
                    asset ballot_fee = vapaee::dex::dao::aux_get_telos_decide_ballot_fee();
                    check(quantity == ballot_fee, create_error_asset2(ERROR_HDT_2, quantity, ballot_fee).c_str());
                    the_concept = vapaee::dex::fees::concept_ballot;
                } else if(memo == "addtoken") {                    
                    // paying token registration fee
                    asset addtoken_fee = vapaee::dex::global::get().regcost;
                    check(quantity == addtoken_fee, create_error_asset2(ERROR_HDT_3, quantity, addtoken_fee).c_str());
                    the_concept = vapaee::dex::fees::concept_addtoken;
                } else {
                    check(false, create_error_string1(ERROR_HDT_4, memo).c_str());
                }

                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HDT_5);

                // register user payed fees
                aux_add_fees(the_concept, sender, quantity, contract);

                PRINT("vapaee::dex::fees::handle_dex_transfer() ...\n");
            }

            // --------------------------------------------------------------

            
        };     
    };
};
