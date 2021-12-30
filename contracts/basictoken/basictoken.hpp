#pragma once

#include <vapaee/base/base.hpp>
// #include <vapaee/dex/modules/error.hpp>

namespace vapaee {

    namespace token {

        class [[eosio::contract]] basictoken : public eosio::contract {
       
            public:
                using contract::contract;

                [[eosio::action]]
                void create( const name&   issuer,
                            const asset&  maximum_supply);

                [[eosio::action]]
                void issue( const name& to, const asset& quantity, const string& memo );

                [[eosio::action]]
                void retire( const asset& quantity, const string& memo );

                [[eosio::action]]
                void transfer( const name&    from,
                                const name&    to,
                                const asset&   quantity,
                                const string&  memo );
                                
                [[eosio::action]]
                void open( const name& owner, const symbol& symbol, const name& ram_payer );

                [[eosio::action]]
                void close( const name& owner, const symbol& symbol );

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

                // using create_action = eosio::action_wrapper<"create"_n, &basictoken::create>;
                // using issue_action = eosio::action_wrapper<"issue"_n, &basictoken::issue>;
                // using retire_action = eosio::action_wrapper<"retire"_n, &basictoken::retire>;
                // using transfer_action = eosio::action_wrapper<"transfer"_n, &basictoken::transfer>;
                // using open_action = eosio::action_wrapper<"open"_n, &basictoken::open>;
                // using close_action = eosio::action_wrapper<"close"_n, &basictoken::close>;

                // Testing porpuses ----------------------------------------
                [[eosio::action]]
                void start() {
                    PRINT("ACTION start\n"); 
                    action( permission_level{get_self(), "active"_n}, get_self(), "action1"_n,  make_tuple() ).send();
                    action( permission_level{get_self(), "active"_n}, get_self(), "action2"_n,  make_tuple() ).send();
                    action( permission_level{get_self(), "active"_n}, get_self(), "action3"_n,  make_tuple() ).send();
                }

                [[eosio::action]]
                void action1() {
                    PRINT("ACTION action1\n"); 
                    action(
                        permission_level{get_self(), "active"_n},
                        name("eosio.token"),
                        name("transfer"), 
                        make_tuple(get_self(), name("echocontract"), asset(1, symbol("TLOS", 4)), string("action1"))
                    ).send();
                }

                [[eosio::action]]
                void action2() {
                    PRINT("ACTION action2\n"); 
                    action(
                        permission_level{get_self(), "active"_n},
                        name("eosio.token"),
                        name("transfer"), 
                        make_tuple(get_self(), name("echocontract"), asset(1, symbol("TLOS", 4)), string("action2"))
                    ).send();
                }

                [[eosio::action]]
                void action3() {
                    PRINT("ACTION action3\n"); 
                    action(
                        permission_level{get_self(), "active"_n},
                        name("eosio.token"),
                        name("transfer"), 
                        make_tuple(get_self(), name("echocontract"), asset(1, symbol("TLOS", 4)), string("action3"))
                    ).send();
                }

                // --------------------------------------------------------
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

    };  // namespace pool

};  // namespace vapaee
