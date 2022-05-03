#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/tables.hpp>
#include <vapaee/token/errors.hpp>
// #include <vapaee/token/modules/rex.hpp>


namespace vapaee {
    namespace token {
        namespace standard {

            inline name get_self() {
                return vapaee::current_contract;
            }
            
            void vote_for_producers() {
                PRINT("vapaee::token::standard::vote_for_producers()");
                
            }

            // init that calls vote_for_producers()
            void action_init() {
                PRINT("vapaee::token::standard::action_init()\n");
                vote_for_producers();
            }

            // ---------
            asset get_supply( const name& token_contract_account, const symbol_code& sym_code ) {
                stats statstable( token_contract_account, sym_code.raw() );
                const auto& st = statstable.get( sym_code.raw() );
                return st.supply;
            }

            asset get_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code ) {
                accounts accountstable( token_contract_account, owner.value );
                const auto& ac = accountstable.get( sym_code.raw() );
                return ac.balance;
            }
            // -----



            // ---------- Standar Token API ----------------------
            // auxiliar functions
            void sub_balance( const name& owner, const asset& value ) {
                accounts from_acnts( get_self(), owner.value );

                const auto& from = from_acnts.get( value.symbol.code().raw(), create_error_asset1(ERROR_SB_1, value).c_str() );
                check( from.balance.amount >= value.amount, 
                    create_error_string3(ERROR_SB_2,
                        owner.to_string(), from.balance.to_string(), value.to_string()) );

                from_acnts.modify( from, owner, [&]( auto& a ) {
                    a.balance -= value;
                });
            }

            void add_balance( const name& owner, const asset& value, const name& ram_payer )
            {
                accounts to_acnts( get_self(), owner.value );
                auto to = to_acnts.find( value.symbol.code().raw() );
                if( to == to_acnts.end() ) {
                    to_acnts.emplace( ram_payer, [&]( auto& a ){
                        a.balance = value;
                    });
                } else {
                    to_acnts.modify( to, same_payer, [&]( auto& a ) {
                        a.balance += value;
                    });
                }
            }

            // -- ACTIONS ----
            void action_create( const name&   issuer,
                            const asset&  maximum_supply )
            {
                check(has_auth(get_self()) || has_auth(issuer), "only issuer can create token (basictoken)");

                auto sym = maximum_supply.symbol;
                check( sym.is_valid(), "invalid symbol name (basictoken)" );
                check( maximum_supply.is_valid(), "invalid supply (basictoken)");
                check( maximum_supply.amount > 0, "max-supply must be positive (basictoken)");

                stats statstable( get_self(), sym.code().raw() );
                auto existing = statstable.find( sym.code().raw() );
                check( existing == statstable.end(), (string("token with symbol already exists (basictoken): ") + sym.code().to_string()).c_str() );

                statstable.emplace( get_self(), [&]( auto& s ) {
                    s.supply.symbol = maximum_supply.symbol;
                    s.max_supply    = maximum_supply;
                    s.issuer        = issuer;
                });
            }


            void action_issue( const name& to, const asset& quantity, const string& memo )
            {
                auto sym = quantity.symbol;
                check( sym.is_valid(), "invalid symbol name (basictoken)" );
                check( memo.size() <= 256, "memo has more than 256 bytes (basictoken)" );

                stats statstable( get_self(), sym.code().raw() );
                auto existing = statstable.find( sym.code().raw() );
                check( existing != statstable.end(), "token with symbol does not exist, create token before issue (basictoken)" );
                const auto& st = *existing;
                check( to == st.issuer, "tokens can only be issued to issuer account (basictoken)" );

                require_auth( st.issuer );
                check( quantity.is_valid(), "invalid quantity (basictoken)" );
                check( quantity.amount > 0, "must issue positive quantity (basictoken)" );

                check( quantity.symbol == st.supply.symbol, create_error_asset2(ERROR_AI_7, quantity, st.supply).c_str() );
                check( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply (basictoken)");

                statstable.modify( st, same_payer, [&]( auto& s ) {
                    s.supply += quantity;
                });

                add_balance( st.issuer, quantity, st.issuer );
            }

            void action_retire( const asset& quantity, const string& memo )
            {
                auto sym = quantity.symbol;
                check( sym.is_valid(), create_error_symcode1(ERROR_AR_1, sym.code()).c_str() );
                check( memo.size() <= 256, create_error_string1(ERROR_AR_2, memo).c_str() );

                stats statstable( get_self(), sym.code().raw() );
                auto existing = statstable.find( sym.code().raw() );
                check( existing != statstable.end(), create_error_symcode1(ERROR_AR_3, sym.code()).c_str() );
                const auto& st = *existing;

                require_auth( st.issuer );
                check( quantity.is_valid(), create_error_asset1(ERROR_AR_4, quantity).c_str() );
                check( quantity.amount > 0, create_error_asset1(ERROR_AR_5, quantity).c_str() );

                check( quantity.symbol == st.supply.symbol, create_error_asset2(ERROR_AR_6, quantity, st.supply).c_str() );

                statstable.modify( st, same_payer, [&]( auto& s ) {
                    s.supply -= quantity;
                });

                sub_balance( st.issuer, quantity );
            }

            void action_transfer( const name& from, const name& to, const asset& quantity, const string& memo)
            {

                PRINT("--> ACTION(",get_self().to_string(),"::transfer) <-- from:", from.to_string(), " to:", to.to_string(), " quantity:", quantity.to_string(), " memo:", memo,
                    quantity.amount <= 0 ? " ERROR: quantity.amount <= 0" : ""
                );
                check( from != to, create_error_name1(ERROR_AT_1, to).c_str() );
                require_auth( from );
                check( is_account( to ), create_error_name1(ERROR_AT_2, to).c_str() );
                auto sym = quantity.symbol.code();
                stats statstable( get_self(), sym.raw());
                const auto& st = statstable.get( sym.raw(), create_error_string2(ERROR_AT_3, get_self().to_string(), sym.to_string()).c_str());

                require_recipient( from );
                require_recipient( to );

                check( quantity.is_valid(), "invalid quantity (basictoken)" );
                // check( quantity.amount > 0, 
                //     vapaee::dex::error::create_error_asset1(
                //         "must transfer positive quantity (basictoken)",
                //         quantity
                //     )        
                // );

                check( quantity.symbol == st.supply.symbol, "symbol precision mismatch (basictoken)" );
                check( memo.size() <= 256, "memo has more than 256 bytes (basictoken)" );

                auto payer = has_auth( to ) ? to : from;

                sub_balance( from, quantity );
                add_balance( to, quantity, payer );
            }

            void action_open( const name& owner, const symbol& symbol, const name& ram_payer )
            {
                require_auth( ram_payer );
                PRINT("vapaee::token::standard::action_open()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" symbol: ", symbol.code().to_string(), "\n");
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");
                PRINT(" -> get_self(): ", get_self().to_string(), "\n");
            
                check( is_account( owner ), create_error_name1(ERROR_AO_1, owner).c_str() );
            
                auto sym_code_raw = symbol.code().raw();
                stats statstable( get_self(), sym_code_raw );
                const auto& st = statstable.get( sym_code_raw, create_error_string2(ERROR_AO_2,
                    get_self().to_string(),
                    symbol.code().to_string()
                ).c_str() );
                check( st.supply.symbol == symbol, create_error_asset2(ERROR_AO_3,
                    asset(0, st.supply.symbol),
                    asset(0, symbol)
                ).c_str() );
            
                accounts acnts( get_self(), owner.value );
                auto it = acnts.find( sym_code_raw );
                if( it == acnts.end() ) {
                    acnts.emplace( ram_payer, [&]( auto& a ){
                        a.balance = asset{0, symbol};
                    });
                }
            }

            void action_close( const name& owner, const symbol& symbol )
            {
                require_auth( owner );
                accounts acnts( get_self(), owner.value );
                auto it = acnts.find( symbol.code().raw() );
                check( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect. (basictoken)" );
                check( it->balance.amount == 0, "Cannot close because the balance is not zero. (basictoken)" );
                acnts.erase( it );
            }


        };     
    };
};
