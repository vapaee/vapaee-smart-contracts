#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/token/modules/standard.hpp>

namespace vapaee {

    namespace token {

        class [[eosio::contract]] basictoken : public eosio::contract {
       
            public:
                using contract::contract;                

                basictoken(name receiver, name code, datastream<const char*> ds):
                    contract(receiver, code, ds)
                    { vapaee::current_contract = receiver; }


                ACTION create( const name&   issuer,
                            const asset&  maximum_supply) {
                    PRINT("\nACTION basictoken::create() ------------------\n");
                    vapaee::token::standard::action_create(issuer, maximum_supply);
                }

                ACTION issue( const name& to, const asset& quantity, const string& memo ) {
                    PRINT("\nACTION basictoken::issue() ------------------\n");
                    vapaee::token::standard::action_issue(to, quantity, memo);
                }

                ACTION retire( const asset& quantity, const string& memo ) {
                    PRINT("\nACTION basictoken::retire() ------------------\n");
                    vapaee::token::standard::action_retire(quantity, memo);
                }

                ACTION transfer(const name& from, const name& to, const asset& quantity, const string& memo) {
                    // PRINT("\nACTION basictoken::transfer() ------------------\n");
                    vapaee::token::standard::action_transfer(from, to, quantity, memo);
                }
                                
                ACTION open( const name& owner, const symbol& symbol, const name& ram_payer ) {
                    PRINT("\nACTION basictoken::open() ------------------\n");
                    vapaee::token::standard::action_open(owner, symbol, ram_payer);
                }

                ACTION close( const name& owner, const symbol& symbol ) {
                    PRINT("\nACTION basictoken::close() ------------------\n");
                    vapaee::token::standard::action_close(owner, symbol);
                }
                
                static asset get_supply( const name& token_contract_account, const symbol_code& sym_code )
                {
                    stats statstable( token_contract_account, sym_code.raw() );
                    const auto& st = statstable.get( sym_code.raw() );
                    return st.supply;
                }

                static asset get_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
                {
                    accounts accountstable( token_contract_account, owner.value );
                    const auto& ac = accountstable.get( sym_code.raw() );
                    return ac.balance;
                }

            private:
                TABLE account {
                    asset    balance;

                    uint64_t primary_key()const { return balance.symbol.code().raw(); }
                };

                TABLE currency_stats {
                    asset    supply;
                    asset    max_supply;
                    name     issuer;

                    uint64_t primary_key()const { return supply.symbol.code().raw(); }
                };

                typedef eosio::multi_index< "accounts"_n, account > accounts;
                typedef eosio::multi_index< "stat"_n, currency_stats > stats;

                void sub_balance( const name& owner, const asset& value );
                void add_balance( const name& owner, const asset& value, const name& ram_payer );

        };  // contract class

    };  // namespace tooken

};  // namespace vapaee
