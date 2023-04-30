#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/modules/wrapper.hpp>
#include <vapaee/token/modules/utils.hpp>

namespace vapaee {
    namespace pool {

        namespace utils {

            inline name get_self() {
                return vapaee::pool::contract;
            }
            
            void send_swap(const name& from, const asset& quantity, const symbol_code& token_o_receive, const name& recipiant, const string& memo) {
                PRINT("vapaee::pool::utils::send_swap()\n");
                PRINT("  from: ", from.to_string(), "\n");
                PRINT("  quantity: ", quantity.to_string(), "\n");
                PRINT("  token_o_receive: ", token_o_receive.to_string(), "\n");
                PRINT("  recipiant: ", recipiant.to_string(), "\n");
                PRINT("  memo: ", memo, "\n");
                

                asset supply = vapaee::token::wrapper::get_token_supply(token_o_receive, "ERR-SSWAP-01");
                asset zero_balance = asset(0, supply.symbol);

                PRINT("  supply: ", supply.to_string(), "\n");
                PRINT("  zero_balance: ", zero_balance.to_string(), "\n");

                string path = get_self().to_string()+"/"+token_o_receive.to_string();
                string swap_memo = string("openpool.v1,")+path+","+zero_balance.to_string()+","+recipiant.to_string()+","+memo ;

                PRINT(" > swap_memo: ", swap_memo.c_str(), "\n");

// if (zero_balance.to_string() == string("0 TLOS")) {
//     check(false, "AAAAAAAAAAAAAAAAA");
// }


                vapaee::token::utils::send_transfer_tokens(
                    from,
                    get_self(),
                    quantity,
                    swap_memo
                );
            }
            void send_swap(const asset& quantity, const symbol_code& token_o_receive, const name& recipiant, const string& memo) {
                send_swap(vapaee::current_contract, quantity, token_o_receive, recipiant, memo);
            }

        }; 
    };
};
