#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/dispatcher.spp>
#include <vapaee/wrap/modules/handler.hpp>
#include <vapaee/wrap/modules/investments.hpp>
#include <vapaee/wrap/modules/lendings.hpp>
#include <vapaee/wrap/modules/maintenance.hpp>
#include <vapaee/wrap/modules/voter.hpp>


namespace vapaee {

    CONTRACT teloswrapper : public eosio::contract {

        private:
            #include <vapaee/wrap/tables.all.hpp>

        public:
            using contract::contract;

            teloswrapper(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }

            // do maintenance
            ACTION maintenance(name credits_to) {
                MAINTENANCE();
                PRINT("\nACTION teloswrapper.maintenance() ------------------\n");

                require_auth(credits_to);

                vapaee::wrap::maintenance::action_do_maintenance(credits_to);
            }

            // initialize empty loan (user's RAM)
            ACTION initloan(name owner) {
                MAINTENANCE();
                PRINT("\nACTION teloswrapper.initloan() ------------------\n");

                require_auth(owner);

                vapaee::wrap::lendings::action_init_loan_ram(owner);
            }

            // cancel loan id
            ACTION cancelloan(name owner, asset quantity, uint64_t loan_id) {
                MAINTENANCE();
                PRINT("\nACTION teloswrapper.cancelloan() ------------------\n");

                require_auth(get_self());

                vapaee::wrap::lendings::handle_cancel_user_loan(owner, quantity, loan_id);
            }

            // cancel loan id
            ACTION redeem(name from, asset quantity) {
                MAINTENANCE();
                PRINT("\nACTION teloswrapper.redeem() ------------------\n");

                require_auth(get_self());

                vapaee::wrap::lendings::handle_redeem(from, quantity);
            }

            // process quantity as profits (private function)
            ACTION profits(asset quantity) {
                MAINTENANCE();
                PRINT("\nACTION teloswrapper.profits() ------------------\n");

                require_auth(get_self());

                vapaee::wrap::investments::action_profits(quantity);
            }

            // initialize the contract
            ACTION init() {
                MAINTENANCE();
                PRINT("\nACTION teloswrapper.init() ------------------\n");

                require_auth(get_self());

                vapaee::wrap::voter::action_init();
            }

            // updates the last snapshot.after 
            /*ACTION updateshot() {
                MAINTENANCE();
                PRINT("\nACTION teloswrapper.updateshot() ------------------\n");

                require_auth(get_self());
                
                vapaee::wrap::global::action_update_last_snapshot();
            }*/


            HANDLER htransfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER teloswrapper.htransfer() ------------------\n");

                // memo can be: "loan <percent>", "cancel <number> <quantity>" or "profits"
                vapaee::wrap::handler::handle_wrap_transfer(
                     from, to, quantity, memo, get_first_receiver());
            }

    };  // contract class



};  // namespace vapaee
