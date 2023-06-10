#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/pay/modules/utils.hpp>
#include <vapaee/token/modules/utils.hpp>

namespace vapaee {
    namespace atwitter {
        namespace handler {

            inline name get_self() {
                return vapaee::atwitter::contract;
            }

            void handle_atwitter_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::token::handler::handle_atwitter_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");
                PRINT(" tokencontract: ", tokencontract.to_string(), "\n");

                if (vapaee::base::global::
                    handler_should_ignore_transfer(from, to, quantity, memo, tokencontract)
                ) return;

                // check if the token is ACORN
                check(
                    tokencontract == vapaee::atwitter::acorn_contract &&
                    quantity.symbol == vapaee::atwitter::acorn_symbol,
                    create_error_string2(
                        "Only ACORN tokens are supported for twitting. You sent: ",
                        tokencontract.to_string(),
                        quantity.to_string()
                    ).c_str());

                asset return_asset = asset(1, vapaee::atwitter::acorn_symbol);
                string return_memo = string("You sent ") + quantity.to_string() + " ACORN tokens to @AcornTwitter";
                vapaee::token::utils::send_transfer_tokens(get_self(), from, return_asset, return_memo, tokencontract);

                asset rex_quantity = quantity - return_asset;

                // send
                string target = "Acorn Twitter";
                vapaee::pay::utils::send_payment_transfer(target, rex_quantity, tokencontract);
                
                PRINT("vapaee::token::handler::handle_atwitter_transfer()...\n");

            }

        };     
    };
};
