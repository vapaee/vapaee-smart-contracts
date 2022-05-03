#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/token/modules/standard.hpp>
#include <vapaee/token/modules/handler.hpp>

namespace vapaee {

    CONTRACT vapaeetokens : public eosio::contract {

        private:
            #include <vapaee/token/tables.all.hpp>
            
        public:
            using contract::contract;

            vapaeetokens(name receiver, name code, datastream<const char*> ds):
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }

            ACTION create( const name&   issuer,
                        const asset&  maximum_supply) {
                vapaee::token::standard::action_create(issuer, maximum_supply);
            }

            ACTION issue( const name& to, const asset& quantity, const string& memo ) {
                vapaee::token::standard::action_issue(to, quantity, memo);
            }

            ACTION retire( const asset& quantity, const string& memo ) {
                vapaee::token::standard::action_retire(quantity, memo);
            }

            ACTION transfer(const name& from, const name& to, const asset& quantity, const string& memo) {
                vapaee::token::standard::action_transfer(from, to, quantity, memo);
            }
                            
            ACTION open( const name& owner, const symbol& symbol, const name& ram_payer ) {
                vapaee::token::standard::action_open(owner, symbol, ram_payer);
            }

            ACTION close( const name& owner, const symbol& symbol ) {
                vapaee::token::standard::action_close(owner, symbol);
            }
            
    };  // contract class

};  // namespace vapaee
