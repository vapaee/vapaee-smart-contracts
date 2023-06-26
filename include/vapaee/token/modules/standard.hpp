#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/tables.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/token/modules/utils.hpp>

namespace vapaee {
    namespace token {
        namespace issuance {
            name get_current_issuer_for(const symbol& sym);
        };
    };
};

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

            // holders registration --
            void register_holder(const name& holder, const symbol_code& sym_code, const name& ram_payer) {
                PRINT("vapaee::token::standard::register_holder()\n");
                holders holders_table(get_self(), sym_code.raw());
                auto it = holders_table.find(holder.value);
                if (it == holders_table.end()) {
                    holders_table.emplace(ram_payer, [&](auto &row) {
                        row.account = holder;
                    });
                }
            }

            void remove_holder(const name& holder, const symbol_code& sym_code) {
                PRINT("vapaee::token::standard::remove_holder()\n");
                holders holders_table(get_self(), sym_code.raw());
                auto it = holders_table.find(holder.value);
                if (it != holders_table.end()) {
                    holders_table.erase(it);
                }
            }

            // ---------
            asset get_supply( const name& token_contract_account, const symbol_code& sym_code, const string& error ) {
                stats statstable( token_contract_account, sym_code.raw() );
                const auto& st = statstable.get( sym_code.raw(), error.c_str() );
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

                const auto& from = from_acnts.get( value.symbol.code().raw(),
                    create_error_string2(
                        ERROR_SB_1,
                        owner.to_string(),
                        value.to_string()
                    ).c_str()
                );
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
            asset get_balance(const name& owner, const symbol& symbol) {
                
                accounts to_acnts( get_self(), owner.value );
                auto to = to_acnts.find( symbol.code().raw() );
                asset balance = asset(0, symbol);
                if( to != to_acnts.end() ) {
                    balance = to->balance;
                }
                return balance;
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
                    has_auth(name("tokenissuer"))        ||
                    has_auth(vapaee::bgbox::contract)    ||
                    has_auth(vapaee::pay::contract)      ||
                    has_auth(vapaee::cnt::contract)      ||
                    has_auth(vapaee::cat::contract)      ||
                    has_auth(vapaee::local::contract)    ||
                    has_auth(vapaee::style::contract)    ||
                    has_auth(vapaee::token::contract)    ||
                    has_auth(vapaee::dex::contract)      ||
                    has_auth(vapaee::book::contract)     ||
                    has_auth(vapaee::pool::contract)     ||
                    has_auth(vapaee::wrap::contract)
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

                name issuer = st.issuer;
                issuers issuerstable( get_self(), sym.code().raw() );
                auto ptr = issuerstable.find( to.value );
                if (ptr != issuerstable.end()) {
                    issuer = ptr->issuer;

                    if (ptr->max.amount == 0) {
                        // issuer is allowed to issue with no restrictions
                    } else {
                        // Check if this account has enough available allowed to issue
                        check( quantity.amount <=  ptr->max.amount,
                            create_error_name1("ERR-ATS-01: quantity exceeds available supply for this issuer", issuer).c_str()
                        );
                        // We update the max available for this account to issue
                        if (quantity.amount == ptr->max.amount) {
                            // issuer is reached maximu allowed so it must be erased
                            issuerstable.erase(*ptr);
                        } else {
                            // there are enough available so we decrement it
                            issuerstable.modify(*ptr, issuer, [&]( auto& s ) {
                                s.max -= quantity;
                            });
                        }
                    }
                }

                check( to == issuer, "tokens can only be issued any issuer account (vapaeetokens)" );
                require_auth( issuer);

                check( quantity.is_valid(), "invalid quantity (vapaeetokens)" );
                check( quantity.amount > 0, "must issue positive quantity (vapaeetokens)" );

                check( quantity.symbol == st.supply.symbol, create_error_asset2(ERROR_AI_7, quantity, st.supply).c_str() );
                check( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply (vapaeetokens)");

                statstable.modify( st, same_payer, [&]( auto& s ) {
                    s.supply += quantity;
                });

                add_balance( issuer, quantity, issuer );

                // assert token registration
                if (get_self() == vapaee::token::contract) {
                    vapaee::token::utils::assert_token_registration(name("add"), st.supply, vapaee::token::contract);
                }

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

                name issuer = vapaee::token::issuance::get_current_issuer_for(quantity.symbol);
                require_auth( issuer );
                check( quantity.is_valid(), create_error_asset1(ERROR_AR_4, quantity).c_str() );
                check( quantity.amount > 0, create_error_asset1(ERROR_AR_5, quantity).c_str() );

                check( quantity.symbol == st.supply.symbol, create_error_asset2(ERROR_AR_6, quantity, st.supply).c_str() );

                statstable.modify( st, same_payer, [&]( auto& s ) {
                    s.supply -= quantity;
                });

                sub_balance( issuer, quantity );

                // assert token registration
                if (get_self() == vapaee::token::contract) {
                    vapaee::token::utils::assert_token_registration(name("add"), st.supply, vapaee::token::contract);
                }             
            }

            void action_transfer( const name& from, const name& to, const asset& quantity, const string& memo)
            {

                PRINT("--> ACTION(",get_self().to_string(),"::transfer) <-- from:", from.to_string(), " to:", to.to_string(), " quantity:", quantity.to_string(), " memo:", memo,
                    quantity.amount <= 0 ? " ERROR: quantity.amount <= 0" : ""
                );
                check( from != to, create_error_name1(ERROR_AT_1, to).c_str() );
                // allow vapaeetokens to force transfer
                name ram_payer = get_self();
                if (!has_auth(get_self())) {
                    require_auth( from );
                    ram_payer = from;
                }
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

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

                

                
                PRINT(" checkpoint 1\n");
                sub_balance( from, quantity );
                PRINT(" checkpoint 2\n");
                add_balance( to, quantity, ram_payer );
                PRINT(" checkpoint 3\n");

                // assert holder registration
                if (get_self() == vapaee::token::contract) {  
                    register_holder(from, quantity.symbol.code(), from);
                }
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
                // assert holder registration
                if (get_self() == vapaee::token::contract) {
                    register_holder(owner, symbol.code(), ram_payer);
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

                // assert holder registration
                if (get_self() == vapaee::token::contract) {            
                    remove_holder(owner, symbol.code());
                }                
            }


            // Hot fix to change tokens precision ----------------
            void aux_accounts_change_precision( const name& owner, const symbol_code& sym_code, const uint8_t precision ) {
                accounts acnts( get_self(), owner.value );
                auto it = acnts.find( sym_code.raw() );
                if( it != acnts.end() ) {
                    acnts.modify( it, same_payer, [&]( auto& a ) {
                        a.balance = vapaee::utils::asset_change_precision(a.balance, precision);
                    });
                }
            }

            void aux_stat_change_precision(const symbol_code& sym_code, const uint8_t precision ) {
                stats statstable( get_self(), sym_code.raw() );
                auto it = statstable.find( sym_code.raw() );
                if( it != statstable.end() ) {
                    statstable.modify( it, same_payer, [&]( auto& s ) {
                        s.supply     = vapaee::utils::asset_change_precision(s.supply, precision);
                        s.max_supply = vapaee::utils::asset_change_precision(s.max_supply, precision);
                    });
                }
            }
            void action_hotfix() {

                // we need to change the EUROT symbol precision to 4 and the KOINE symbol precision to 6
                // change stat table

                // symbol_code eurot = symbol_code("EUROT");
                // aux_stat_change_precision(eurot, 4);

                // symbol_code koine = symbol_code("KOINE");
                // aux_stat_change_precision(koine, 4);

                
                // stats statstable( get_self(), koine.raw() );
                // auto it = statstable.find( koine.raw() );
                // if( it != statstable.end() ) {
                //     statstable.modify( it, same_payer, [&]( auto& s ) {
                //         s.max_supply = asset(200000000000000000,s.max_supply.symbol);
                //     });
                // }

                // Vamos a vaciar la tabla foreign
                foreign foreign_table(get_self(), get_self().value);
                for (auto it = foreign_table.begin(); it != foreign_table.end(); it = foreign_table.begin()) {
                    foreign_table.erase(it);
                }

                // lista de EUROT holders

                // coinkoinonos
                // teloseurowlt
                // pabloeverest
                // koinonospool
                // icaronutriti
                // viterbotelos
                // uy
                // faustoandree
                // vapaee
                // aux_accounts_change_precision(name("coinkoinonos"), eurot, 4);
                // aux_accounts_change_precision(name("teloseurowlt"), eurot, 4);
                // aux_accounts_change_precision(name("pabloeverest"), eurot, 4);
                // aux_accounts_change_precision(name("koinonospool"), eurot, 4);
                // aux_accounts_change_precision(name("icaronutriti"), eurot, 4);
                // aux_accounts_change_precision(name("viterbotelos"), eurot, 4);
                // aux_accounts_change_precision(name("uy"), eurot, 4);
                // aux_accounts_change_precision(name("faustoandree"), eurot, 4);
                // aux_accounts_change_precision(name("vapaee"), eurot, 4);


                // lista de KOINE holders

                // pabloeverest
                // coinkoinonos
                // koinonospool
                // icaronutriti
                // viterbotelos
                // uy
                // faustoandree
                // vapaee
                // aux_accounts_change_precision(name("pabloeverest"), koine, 6);
                // aux_accounts_change_precision(name("coinkoinonos"), koine, 6);
                // aux_accounts_change_precision(name("koinonospool"), koine, 6);
                // aux_accounts_change_precision(name("icaronutriti"), koine, 6);
                // aux_accounts_change_precision(name("viterbotelos"), koine, 6);
                // aux_accounts_change_precision(name("uy"), koine, 6);
                // aux_accounts_change_precision(name("faustoandree"), koine, 6);
                // aux_accounts_change_precision(name("vapaee"), koine, 6);


                



            }
        };     
    };
};

#include <vapaee/token/modules/issuance.hpp>
