#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/pay/modules/deposit.hpp>
#include <vapaee/pay/modules/handler.hpp>

namespace vapaee {

    CONTRACT vapaeepayhub : public eosio::contract {

        private:
            #include <vapaee/pay/tables.all.hpp>

        public:
            using contract::contract;


            vapaeepayhub(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }


            ACTION withdraw(const name& owner, const asset&  maximum_supply) {
                vapaee::pay::deposit::action_withdraw(owner, maximum_supply);
            }


            // TODO: implement deposit
            [[eosio::on_notify("*::transfer")]]
            void htransfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER vapaeepayhub.htransfer() ------------------\n");


                // TODO
                vapaee::pay::handler::handle_pay_transfer(
                     from, to, quantity, memo, get_first_receiver());
            }
            
    };  // contract class

};  // namespace vapaee
