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

                name ram_payer = owner;
                if (!has_auth(ram_payer)) {
                    ram_payer = get_self();
                }

                from_acnts.modify( from, ram_payer, [&]( auto& a ) {
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
                check(has_auth(get_self()) || has_auth(issuer), "only issuer can create token (vapaeetokens)");
                name rampayer = issuer;
                if (has_auth(get_self())) {
                    rampayer = get_self();
                }

                auto sym = maximum_supply.symbol;
                check( sym.is_valid(), "invalid symbol name (vapaeetokens)" );
                check( maximum_supply.is_valid(), "invalid supply (vapaeetokens)");
                check( maximum_supply.amount > 0, "max-supply must be positive (vapaeetokens)");

                stats statstable( get_self(), sym.code().raw() );
                auto existing = statstable.find( sym.code().raw() );
                check( existing == statstable.end(), (string("token with symbol already exists (vapaeetokens): ") + sym.code().to_string()).c_str() );

                statstable.emplace( rampayer, [&]( auto& s ) {
                    s.supply.symbol = maximum_supply.symbol;
                    s.max_supply    = maximum_supply;
                    s.issuer        = issuer;
                });

                check(
                    has_auth(vapaee::bgbox::contract)    ||
                    has_auth(vapaee::pay::contract)      ||
                    has_auth(vapaee::cnt::contract)      ||
                    has_auth(vapaee::cat::contract)      ||
                    has_auth(vapaee::str::contract)      ||
                    has_auth(vapaee::tprofile::contract) ||
                    has_auth(vapaee::author::contract)   ||
                    has_auth(vapaee::style::contract)    ||
                    has_auth(vapaee::token::contract)    ||
                    has_auth(vapaee::ttracker::contract) ||
                    has_auth(vapaee::dex::contract)      ||
                    has_auth(vapaee::book::contract)     ||
                    has_auth(vapaee::pool::contract)     ||
                    has_auth(vapaee::wrap::contract)     ||
                    has_auth(vapaee::echo::contract)     ||
                    has_auth(vapaee::nft::contract)
                , "This action has been disabled for the moment. Please join our telegram group @vapaee_dex if you have any questions. (vapaeetokens)");
            }


            void action_issue( const name& to, const asset& quantity, const string& memo )
            {
                auto sym = quantity.symbol;
                check( sym.is_valid(), "invalid symbol name (vapaeetokens)" );
                check( memo.size() <= 256, "memo has more than 256 bytes (vapaeetokens)" );

                stats statstable( get_self(), sym.code().raw() );
                auto existing = statstable.find( sym.code().raw() );
                check( existing != statstable.end(), "token with symbol does not exist, create token before issue (vapaeetokens)" );
                const auto& st = *existing;
                check( to == st.issuer, "tokens can only be issued to issuer account (vapaeetokens)" );

                require_auth( st.issuer );
                check( quantity.is_valid(), "invalid quantity (vapaeetokens)" );
                check( quantity.amount > 0, "must issue positive quantity (vapaeetokens)" );

                check( quantity.symbol == st.supply.symbol, create_error_asset2(ERROR_AI_7, quantity, st.supply).c_str() );
                check( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply (vapaeetokens)");

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
                // allow vapaeetokens to force transfer
                if (!has_auth(get_self())) {
                    require_auth( from );
                }
                check( is_account( to ), create_error_name1(ERROR_AT_2, to).c_str() );
                auto sym = quantity.symbol.code();
                stats statstable( get_self(), sym.raw());
                const auto& st = statstable.get( sym.raw(), create_error_string2(ERROR_AT_3, get_self().to_string(), sym.to_string()).c_str());

                require_recipient( from );
                require_recipient( to );

                check( quantity.is_valid(), "invalid quantity (vapaeetokens)" );
                check( quantity.amount > 0, 
                    create_error_asset1(
                        "must transfer positive quantity (vapaeetokens)",
                        quantity
                    )        
                );

                check( quantity.symbol == st.supply.symbol, "symbol precision mismatch (vapaeetokens)" );
                check( memo.size() <= 256, "memo has more than 256 bytes (vapaeetokens)" );

                auto payer = has_auth( to ) ? to : from;

                PRINT(" payer: ", payer.to_string(), "\n");
                
                PRINT(" checkpoint 1\n");
                sub_balance( from, quantity );
                PRINT(" checkpoint 2\n");
                add_balance( to, quantity, payer );
                PRINT(" checkpoint 3\n");
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
                check( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect. (vapaeetokens)" );
                check( it->balance.amount == 0, "Cannot close because the balance is not zero. (vapaeetokens)" );
                acnts.erase( it );
            }


        };     
    };
};
