#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/token/modules/standard.hpp>
#include <vapaee/token/modules/handler.hpp>
#include <vapaee/token/modules/issuance.hpp>
#include <vapaee/token/modules/wrapper.hpp>
#include <vapaee/token/modules/debit.hpp>

namespace vapaee {

    CONTRACT vapaeetokens : public eosio::contract {

        private:
            #include <vapaee/token/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>
            
        public:
            using contract::contract;

            string get_version() { return string("1.0.0"); } // vapaeetokens

            // token module

            vapaeetokens(name receiver, name code, datastream<const char*> ds):
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver;  vapaee::current_version = get_version();  }

            // Global module
            ACTION init() {
                PRINT("\nACTION vapaeetokens::init() ------------------\n");
                vapaee::base::global::action_init();
            };                 

            ACTION create( const name&   issuer,
                        const asset&  maximum_supply) {
                PRINT("\nACTION vapaeetokens::create() ------------------\n");
                vapaee::token::standard::action_create(issuer, maximum_supply);
            }

            ACTION issue( const name& to, const asset& quantity, const string& memo ) {
                PRINT("\nACTION vapaeetokens::issue() ------------------\n");
                vapaee::token::standard::action_issue(to, quantity, memo);
            }

            ACTION retire( const asset& quantity, const string& memo ) {
                PRINT("\nACTION vapaeetokens::retire() ------------------\n");
                vapaee::token::standard::action_retire(quantity, memo);
            }

            ACTION transfer(const name& from, const name& to, const asset& quantity, const string& memo) {
                PRINT("\nACTION vapaeetokens::transfer() ------------------\n");
                vapaee::token::standard::action_transfer(from, to, quantity, memo);
            }
                            
            ACTION open( const name& owner, const symbol& symbol, const name& ram_payer ) {
                PRINT("\nACTION vapaeetokens::open() ------------------\n");
                vapaee::token::standard::action_open(owner, symbol, ram_payer);
            }

            ACTION close( const name& owner, const symbol& symbol ) {
                PRINT("\nACTION vapaeetokens::close() ------------------\n");
                vapaee::token::standard::action_close(owner, symbol);
            }

            // issuance module

            ACTION chissuer( const name& new_issuer, const symbol_code& token) {
                PRINT("\nACTION vapaeetokens::chissuer() ------------------\n");
                vapaee::token::issuance::action_chissuer(new_issuer, token);
            }

            ACTION issuer(const name& action, const name& theissuer, const asset& max) {
                PRINT("\nACTION vapaeetokens::issuer() ------------------\n");
                vapaee::token::issuance::action_issuer(action, theissuer, max);
            }
            
            // wrapper module

            ACTION deposit(const name& owner, const asset& quantity, const name& token_contract) {
                PRINT("\nACTION vapaeetokens::deposit() ------------------\n");
                vapaee::token::wrapper::action_deposit(owner, quantity, token_contract);
            }

            ACTION withdraw(const name& owner, const asset& quantity, const string& notes) {
                PRINT("\nACTION vapaeetokens::withdraw() ------------------\n");
                vapaee::token::wrapper::action_withdraw(owner, quantity, notes);
            }

            [[eosio::on_notify("*::transfer")]]
            void handle_transfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER vapaeetokens::handle_transfer() ------------------\n");

                vapaee::token::handler::handle_token_transfer(from, to, quantity, memo, get_first_receiver());

            }

            // debit module

            ACTION debit(name from, name collector, asset quantity, string memo) {
                PRINT("\nACTION vapaeetokens::debit() ------------------\n");
                vapaee::token::debit::action_debit(from, collector, quantity, memo);
            }

            ACTION mngdebit(
                name action, 
                name owner,
                name collector, 
                asset max_total, 
                asset max_debt, 
                double max_perc, 
                uint64_t sec_to_expire
            ) {
                PRINT("\nACTION vapaeetokens::mngdebit() ------------------\n");
                vapaee::token::debit::action_manage_debit(action, owner, collector, max_total, max_debt, max_perc, sec_to_expire);
            }



    };  // contract class

};  // namespace vapaee
