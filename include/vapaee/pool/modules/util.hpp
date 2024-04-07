#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/modules/wrapper.hpp>
#include <vapaee/token/modules/utils.hpp>
#include <vapaee/dex/modules/token.hpp>

namespace vapaee {
    namespace pool {

        namespace util {

            void send_swap(
                const name& from,
                const asset& quantity,
                const symbol_code& token_to_receive,
                const name& recipiant,
                const string& memo,
                const name& converter,
                const string& path
            ) {
                PRINT("vapaee::pool::util::send_swap()\n");
                PRINT("  from: ", from.to_string(), "\n");
                PRINT("  quantity: ", quantity.to_string(), "\n");
                PRINT("  token_to_receive: ", token_to_receive.to_string(), "\n");
                PRINT("  recipiant: ", recipiant.to_string(), "\n");
                PRINT("  memo: ", memo, "\n");
                PRINT("  converter: ", converter.to_string(), "\n");
                

                asset supply = vapaee::token::wrapper::get_token_supply(token_to_receive, "ERR-SSWAP-01");
                asset zero_balance = asset(0, supply.symbol);

                PRINT("  supply: ", supply.to_string(), "\n");
                PRINT("  zero_balance: ", zero_balance.to_string(), "\n");

                string swap_memo = string("openpool.v1,")+path+","+zero_balance.to_string()+","+recipiant.to_string()+","+memo ;

                PRINT(" > swap_memo: ", swap_memo.c_str(), "\n");

                name contract = vapaee::dex::token::get_asset_token_contract(quantity);
                vapaee::token::utils::send_transfer_tokens(
                    from,
                    converter,
                    quantity,
                    swap_memo,
                    contract
                );
            }
            void send_swap(const asset& quantity, const symbol_code& token_to_receive, const name& recipiant, const string& memo) {
                name converter = vapaee::current_contract;
                string path = converter.to_string()+"/"+token_to_receive.to_string();
                send_swap(vapaee::current_contract, quantity, token_to_receive, recipiant, memo, converter, path);
            }
            void send_swap(const asset& quantity, const symbol_code& token_to_receive, const name& recipiant, const string& memo, const name& converter) {
                string path = converter.to_string()+"/"+token_to_receive.to_string();
                send_swap(vapaee::current_contract, quantity, token_to_receive, recipiant, memo, converter, path);
            }

        }; 
    };
};
