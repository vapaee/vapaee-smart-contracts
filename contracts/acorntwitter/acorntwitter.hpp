#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/atwitter/modules/handler.hpp>

namespace vapaee {

    CONTRACT acorntwitter : public eosio::contract {

        private:
            #include <vapaee/atwitter/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>
            
        public:
            using contract::contract;

            string get_version() { return string("0.0.1"); } // acorntwitter-0.0.1 - introducing contract acorntwitter

            // token module

            acorntwitter(name receiver, name code, datastream<const char*> ds):
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver;  vapaee::current_version = get_version();  }

            // Global module
            ACTION init() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::init() ------------------\n");
                vapaee::base::global::action_init();
            };

            [[eosio::on_notify("*::transfer")]]
            void handle_transfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER acorntwitter::handle_transfer() ------------------\n");

                vapaee::atwitter::handler::handle_atwitter_transfer(from, to, quantity, memo, get_first_receiver());

            }
            
    };  // contract class

};  // namespace vapaee
