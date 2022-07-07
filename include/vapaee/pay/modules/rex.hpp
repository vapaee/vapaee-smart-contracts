#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/tables.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/token/modules/standard.hpp>
#include <vapaee/token/modules/debit.hpp>
#include <vapaee/token/modules/wrapper.hpp>


namespace vapaee {
    namespace pay {
        namespace rex {

            inline name get_self() {
                return vapaee::pay::contract;
            }

            // helppers ---

            std::string vector_to_string(std::vector<name> const &vec) {
                std::string cats = "[]";
                if (!vec.empty()) {
                    cats = string("[");
                    for (int i=0; i<vec.size(); i++) {
                        if (i>0) cats += ", ";
                        cats += vec[i].to_string();
                    }
                    cats += string("]");
                }
                return cats;
            }

            void send_transfer_tokens(const name& from, const name& to, const asset& quantity, const string& memo, const name& contract  ) {
                action(
                    permission_level{get_self(), "active"_n},
                    contract,
                    "transfer"_n,
                    make_tuple(
                        from,
                        to,
                        quantity,
                        memo
                    )
                ).send();
            }

            void send_debit_to_owner(const name& owner, const asset& quantity, const string& memo) {
                action(
                    permission_level{get_self(), "active"_n},
                    vapaee::token::contract,
                    name("debit"),
                    make_tuple(
                        owner,
                        get_self(),
                        quantity,
                        memo
                    )
                ).send();
            }
            
            
            bool get_stakeconfig_for_token(
                bool create,
                const symbol_code& token,
                stakeconfigs_table& config,
                const name& ram_payer,
                const char * error_code
            ) {

                std::vector<name> categories;
                for (int i=0; i<config.categories.size(); i++)
                    categories.push_back(config.categories[i]);

                stakeconfigs stkconf_table(get_self(), token.raw());
                auto config_ptr = stkconf_table.find(token.raw());

                if (config_ptr == stkconf_table.end() && !create) {
                    if (error_code) {
                        check(false, 
                            create_error_string1(
                                string(string(error_code) + ": the stakeconfigs entry for the token was not found. (token): ").c_str(),
                                token.to_string()
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (config_ptr == stkconf_table.end() && create) {
                    stkconf_table.emplace(ram_payer, [&](auto &a){
                        a.token             = config.token;
                        a.admin             = config.admin;
                        a.title             = config.title;
                        a.desc              = config.desc;
                        a.categories        = categories;
                        a.credits_locktime  = config.credits_locktime;
                        a.total_stake       = config.total_stake;
                        a.total_funds       = config.total_funds;
                    });

                    return true;
                }

                if (config_ptr != stkconf_table.end() && create) {
                    stkconf_table.modify(config_ptr, ram_payer, [&](auto &a){
                        a.admin             = config.admin;
                        a.title             = config.title;
                        a.desc              = config.desc;                        
                        a.categories        = categories;
                        a.credits_locktime  = config.credits_locktime;
                        a.total_stake       = config.total_stake;
                        a.total_funds       = config.total_funds;                                                
                    });

                    return true;
                }
                
                config.token = config_ptr->token;
                config.admin = config_ptr->admin;
                config.title = config_ptr->title;
                config.desc  = config_ptr->desc;

                config.categories.clear();
                for (int i=0; i<config_ptr->categories.size(); i++)
                    config.categories.push_back(config_ptr->categories[i]);
                
                config.credits_locktime = config_ptr->credits_locktime;
                config.total_stake = config_ptr->total_stake;
                config.total_funds = config_ptr->total_funds;

                return true;
            }

            
            bool get_stakepool_for_pool_id(
                bool create,
                const symbol_code& token,
                const name& pool_id,
                stakepool_table& pool,
                const name& ram_payer,
                const char * error_code
            ) {

                stakepools stkpool_table(get_self(), token.raw());
                auto pool_ptr = stkpool_table.find(pool_id.value);

                if (pool_ptr == stkpool_table.end() && !create)  {
                    if (error_code) {
                        check(false, 
                            create_error_string2(
                                string(string(error_code) + ": the stakepools entry for the token and pool_id was not found. (token, pool_id): ").c_str(),
                                token.to_string(),
                                pool_id.to_string()
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (pool_ptr == stkpool_table.end() && create) {
                    stkpool_table.emplace(ram_payer, [&](auto &a){
                        a.id         = pool_id;
                        a.token      = token;
                        a.locktime   = pool.locktime;
                        a.title      = pool.title;
                        a.desc       = pool.desc;
                        a.pool_stake = pool.pool_stake;
                        a.pool_funds = pool.pool_funds;
                        a.pool_rex   = pool.pool_rex;
                    });

                    return true;
                };

                if (pool_ptr != stkpool_table.end() && create) {
                    stkpool_table.modify(pool_ptr, ram_payer, [&](auto &a){
                        a.locktime   = pool.locktime;
                        a.title      = pool.title;
                        a.desc       = pool.desc;
                        a.pool_stake = pool.pool_stake;
                        a.pool_funds = pool.pool_funds;
                        a.pool_rex   = pool.pool_rex;
                    });

                    return true;
                };
                
                pool.id         = pool_ptr->id;
                pool.token      = pool_ptr->token;

                pool.locktime   = pool_ptr->locktime;
                pool.title      = pool_ptr->title;
                pool.desc       = pool_ptr->desc;

                pool.pool_stake = pool_ptr->pool_stake;
                pool.pool_funds = pool_ptr->pool_funds;
                pool.pool_rex   = pool_ptr->pool_rex;

                return true;
            }

            
            bool get_owner_staking_for_token(
                bool create,
                const name& owner,
                const symbol_code& token,
                staking_table& stake,
                const name& ram_payer,
                const char * error_code
            ) {

                staking staking_table(get_self(), owner.value);
                auto stake_ptr = staking_table.find(token.raw());

                if (stake_ptr == staking_table.end() && !create) {
                    if (error_code) {
                        check(false, 
                            create_error_string2(
                                string(string(error_code) + ": the owner's staking entry for the token was not found. (owner, token): ").c_str(),
                                owner.to_string(),
                                token.to_string()
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (stake_ptr == staking_table.end() && create) {
                    staking_table.emplace(ram_payer, [&](auto &a){
                        asset supply = vapaee::token::wrapper::get_token_supply(token, 
                            create_error_symcode1(
                                string(string(error_code) + ": could not get toekn supply: ").c_str(),
                                token
                            )
                        );
                        asset zero_token = asset(0, supply.symbol);

                        a.total_stake  = zero_token;
                        a.total_mature = zero_token;
                        a.last_update  = time_point_sec(0);
                    });

                    return true;
                }

                if (stake_ptr != staking_table.end() && create) {
                    staking_table.modify(stake_ptr, ram_payer, [&](auto &a) {
                        a.total_stake  = stake.total_stake;
                        a.total_mature = stake.total_mature;
                        a.last_update  = stake.last_update;                        
                        a.credits.clear();
                        for (int i=0; i<stake.credits.size(); i++)
                            a.credits.push_back(stake.credits[i]);
                    });
                }
                
                stake.total_stake  = stake_ptr->total_stake;
                stake.total_mature = stake_ptr->total_mature;
                stake.last_update  = stake_ptr->last_update;

                stake.credits.clear();
                for (int i=0; i<stake_ptr->credits.size(); i++)
                    stake.credits.push_back(stake_ptr->credits[i]);


                return true;
            }

                        
            bool get_owner_poolstaking_for_pool_id(
                bool create,
                const name& owner,
                const symbol_code& token,
                const name& pool_id,
                poolstaking_table& pool,
                const name& ram_payer,
                const char * error_code
            ) {

                poolstaking poolstaking_table(get_self(), owner.value);
                auto index = poolstaking_table.get_index<name("poolstake")>(); 
                auto pool_ptr = index.find(
                    vapaee::utils::pack( token.raw(), pool_id.value )
                );

                

                if (pool_ptr == index.end() && !create) {
                    if (error_code) {
                        check(false, 
                            create_error_string3(
                                string(string(error_code) + ": the owner's poolstaking entry for the token and pool_id was not found. (owner, token, pool_id): ").c_str(),
                                owner.to_string(),
                                token.to_string(),
                                pool_id.to_string()
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (pool_ptr == index.end() && create) {
                    poolstaking_table.emplace(ram_payer, [&](auto &a){
                        a.id = poolstaking_table.available_primary_key();
                        a.pool_id = pool_id;
                        asset supply = vapaee::token::wrapper::get_token_supply(token, 
                            create_error_symcode1(
                                string(string(error_code) + ": could not get toekn supply: ").c_str(),
                                token
                            )
                        );
                          
                        asset zero_token = asset(0, supply.symbol);
                        asset zero_rex = asset(0, symbol(vapaee::pay::REX_TKN_CODE, supply.symbol.precision()));                            
                        a.stake   = zero_token;
                        a.mature  = zero_token;
                        a.rex     = zero_rex;
                        a.maturing.clear();
                        for (int i=0; i<pool.maturing.size(); i++) {
                            a.maturing.push_back(pool.maturing[i]);
                        }                        
                    });

                    return true;
                }

                if (pool_ptr != index.end() && create) {
                    index.modify(pool_ptr, ram_payer, [&](auto &a){
                        a.stake   = pool.stake;
                        a.mature  = pool.mature;
                        a.rex     = pool.rex;
                        a.maturing.clear();
                        for (int i=0; i<pool.maturing.size(); i++) {
                            a.maturing.push_back(pool.maturing[i]);
                        }                        
                    });

                    return true;
                }

                pool.id      = pool_ptr->id;
                pool.pool_id = pool_ptr->pool_id;
                pool.stake   = pool_ptr->stake;
                pool.rex     = pool_ptr->rex;
                pool.mature  = pool_ptr->mature;

                for (int i=0; i<pool_ptr->maturing.size(); i++) {
                    pool.maturing.push_back(pool_ptr->maturing[i]);
                }
                return true;
            }
          
            // actions ---

            // admin registrates a token for staking or updates staking info
            void action_stakeconfig(
                const name& action,
                const name& admin,
                const symbol_code& token,
                const name& token_contract,
                const string& title,
                const string& desc,
                const std::vector<name>& categories,
                const string& credits_locktime
            ) {
                PRINT("vapaee::pay::rex::action_stakeconfig()\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" token_contract: ", token_contract.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" desc: ", desc.c_str(), "\n");
                int i=0; for(const auto& cat: categories) {
                    PRINT(" categories[",i++,"]: ", cat.to_string(), "\n");
                }
                PRINT(" credits_locktime: ", credits_locktime.c_str(), "\n");

                // require admin auth
                require_auth(admin);
                name ram_payer = admin;

                // check token is debitable and take total supply asset
                string error_1 = create_error_string2(
                    "ERR-ASC-1: can't read supply of token. Trying (symbol, contract): ",
                    token.to_string(),
                    token_contract.to_string()
                );
                string error_2 = create_error_string2(
                    "ERR-ASC-2: This token is not debitable so you can't create a staking config arround it. (symbol, contract): ",
                    token.to_string(),
                    token_contract.to_string()
                );
                vapaee::token::debit::check_token_debitable(token, token_contract, error_2);
                asset supply = vapaee::token::wrapper::get_token_supply(token, error_1);

                stakeconfigs_table stakeconfig;
                stakeconfig.token = token;
                stakeconfig.admin = admin; 
                stakeconfig.title = title;
                stakeconfig.desc = desc;
                stakeconfig.categories = categories;
                stakeconfig.credits_locktime = credits_locktime;
                stakeconfig.total_stake = asset(0, supply.symbol);
                stakeconfig.total_funds = asset(0, supply.symbol);
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);

                // parse credits_locktime
                int days = vapaee::utils::check_integer_from_string(credits_locktime);


                // if action is remove
                if (action == name("remove")) {
                    // TODO: implement remove staking configuration
                    check(false,"ERR-AMD-3: Can't remove staking configuration. This is not allowed ywt. If you still want to remove this entry please contact us at Telegram @vapaee_dex");
                    return;
                }
                
                PRINT("vapaee::pay::rex::action_stakeconfig() ...\n");
            }
            
            // admin creates a new pool or updates it for qa given token
            void action_stakepool(
                const name& action,
                const symbol_code& token,
                const name& pool_id,
                const string& title,
                const string& desc,
                const string& locktime
            ) {
                PRINT("vapaee::pay::rex::action_stakepool()\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" pool_id: ", pool_id.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" desc: ", desc.c_str(), "\n");
                PRINT(" locktime: ", locktime.c_str(), "\n");

                // Get the existing (if any) pointer to the staking confituration
                stakeconfigs stkconf_table(get_self(), token.raw());
                auto config = stkconf_table.find(token.raw());
                check(config != stkconf_table.end(),
                    create_error_symcode1("ERR-ASP-1: stake configuration not found for token: ", token).c_str());

                // require admin auth or contract
                name ram_payer = config->admin;
                if (has_auth(get_self())) {
                    ram_payer = get_self();
                } else {
                    require_auth(config->admin);
                }

                // get current supply for this token
                asset supply = vapaee::token::wrapper::get_token_supply(token,
                    create_error_symcode1("ERR-ASP-2: could not get supply of token: ", token));
                asset zero_token = asset(0, supply.symbol);
                asset zero_rex = asset(0, symbol(vapaee::pay::REX_TKN_CODE, supply.symbol.precision()));
                PRINT(" > supply: ", supply.to_string(), "\n");
                PRINT(" > zero_token: ", zero_token.to_string(), "\n");
                PRINT(" > zero_rex: ", zero_rex.to_string(), "\n");

                stakepool_table stakepool;
                stakepool.token      = token;
                stakepool.id         = pool_id;
                stakepool.locktime   = locktime;
                stakepool.title      = title;
                stakepool.desc       = desc;
                stakepool.pool_stake = zero_token;
                stakepool.pool_funds = zero_token;
                stakepool.pool_rex   = zero_rex;
                get_stakepool_for_pool_id(true, token, pool_id, stakepool, ram_payer, NULL);
                
                // if action is remove
                if (action == name("remove")) {
                    // TODO: implement remove staking configuration
                    check(false,"ERR-AMD-5: Can't remove staking configuration. This is not allowed ywt. If you still want to remove this entry please contact us at Telegram @vapaee_dex");
                    return;
                }

                PRINT("vapaee::pay::rex::action_stakepool() ...\n");
            }

            void action_droponpool(
                const asset& quantity,
                const name& pool_id
            ) {
                PRINT("vapaee::pay::rex::action_droponpool()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" pool_id: ", pool_id.to_string(), "\n");

                require_auth(get_self());
                name ram_payer = get_self();
                symbol_code token = quantity.symbol.code();

                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-ADS-01");

                stakepool_table stakepool;
                get_stakepool_for_pool_id(false, token, pool_id, stakepool, ram_payer, "ERR-ADS-02");

                stakeconfig.total_funds += quantity;
                stakepool.pool_funds += quantity;

                get_stakepool_for_pool_id(true, token, pool_id, stakepool, ram_payer, NULL);
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);

                PRINT("vapaee::pay::rex::action_droponpool() ...\n");
            }

            void action_stake(
                const name& owner,
                const asset& quantity,
                const name& pool_id
            ) {
                PRINT("vapaee::pay::rex::action_stake()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" pool_id: ", pool_id.to_string(), "\n");

                require_auth(owner);
                name ram_payer = owner;
                symbol_code token = quantity.symbol.code();
                asset supply = vapaee::token::wrapper::get_token_supply(token, 
                            create_error_symcode1("ERR-AS-01: could not get supply of the token:", token));
                asset zero_token = asset(0, supply.symbol);
                asset zero_rex = asset(0, symbol(vapaee::pay::REX_TKN_CODE, supply.symbol.precision()));

                // get or create staking entry
                staking_table staking;
                bool exists = get_owner_staking_for_token(false, owner, token, staking, ram_payer, NULL);
                if (!exists) {
                    staking.total_stake        = zero_token;
                    staking.total_mature       = zero_token;
                    staking.last_update  = time_point_sec(0);
                    get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);  
                }

                // get or create poolstaking entry
                poolstaking_table poolstaking;
                exists = get_owner_poolstaking_for_pool_id(false, owner, token, pool_id, poolstaking, ram_payer, NULL);
                if (!exists) {
                    poolstaking.pool_id  = pool_id;
                    poolstaking.stake    = zero_token;
                    poolstaking.mature   = zero_token;
                    poolstaking.rex      = zero_rex;
                    get_owner_poolstaking_for_pool_id(true, owner, token, pool_id, poolstaking, ram_payer, NULL);
                }

                // get staking and pool configurations and state
                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-AS-02");

                stakepool_table stakepool;
                get_stakepool_for_pool_id(false, token, pool_id, stakepool, ram_payer, "ERR-AS-03");

                // asserts
                check (stakepool.pool_funds.symbol.precision() == stakepool.pool_rex.symbol.precision(),
                    create_error_asset2("ERR-AS-04: assets difiere in precision:", stakepool.pool_funds, stakepool.pool_rex).c_str());

                // calculate
                asset stake_deposit    = quantity;
                asset funds_deposit    = quantity;
                double rex_price       = stakepool.rex_price();
                double rex_inverse     = stakepool.rex_inverse();
                asset rex_deposit      = asset(quantity.amount * rex_inverse, stakepool.pool_rex.symbol);
                int days               = vapaee::utils::check_integer_from_string(stakepool.locktime);
                time_point_sec now     = vapaee::dex::global::get_now_time_point_sec();
                time_point_sec mature  = vapaee::dex::global::get_N_days_from_point_sec(now, days);


                // update tables
                stakeconfig.total_stake += stake_deposit;
                stakeconfig.total_funds += funds_deposit;
                stakepool.pool_stake    += stake_deposit;
                stakepool.pool_funds    += funds_deposit;
                stakepool.pool_rex      += rex_deposit;
                staking.total_stake     += stake_deposit;
                poolstaking.rex         += rex_deposit;
                poolstaking.stake       += stake_deposit;
                maturing_funds maturing;
                maturing.stake          = stake_deposit;
                maturing.mature         = mature;
                poolstaking.maturing.push_back(maturing);

                // save tables in RAM
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);
                get_stakepool_for_pool_id(true, token, pool_id, stakepool, ram_payer, NULL);
                get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);
                get_owner_poolstaking_for_pool_id(true, owner, token, pool_id, poolstaking, ram_payer, NULL);

                // debit quantity from owner
                string memo = string("staking ") + quantity.to_string() + " into " + token.to_string() + "-" + pool_id.to_string() + " pool.";
                send_debit_to_owner(owner, quantity, memo);

                PRINT("vapaee::pay::rex::action_stake() ...\n");
            }
            
            void action_mycredits(
                const name& owner,
                const std::vector<std::tuple<name, asset>>& credits
            ) {
                PRINT("vapaee::pay::rex::action_mycredits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                int i=0; for(const auto& c: credits) {
                    PRINT(" credits[",i++,"]: ", std::get<0>(c).to_string(), " - ", std::get<1>(c).to_string(),  "\n");
                }
                
                require_auth(owner);
                name ram_payer = owner;

                check(credits.size() > 0, "ERR-AMC-01: credits can not be an empty list");
                symbol_code token = std::get<1>(credits[0]).symbol.code();

                // get staking configurations and state
                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-AMC-01");

                // get staking entry
                staking_table staking;
                get_owner_staking_for_token(false, owner, token, staking, ram_payer, "ERR-AMC-02");

                // TODO: que apsa si los credits son de diferente symbol?
                asset suma = asset(0, staking.total_stake.symbol);
                name category;
                asset stake;
                std::vector<credit_assignation> formated_credits;
                for(const auto& c: credits) {
                    category = std::get<0>(c);
                    stake = std::get<1>(c);
                    suma += stake;
                    auto pointer = std::find(stakeconfig.categories.begin(), stakeconfig.categories.end(), category);

                    credit_assignation assign = {category, stake};
                    formated_credits.push_back(assign);

                    check(pointer != stakeconfig.categories.end(), 
                        create_error_string2(
                            "ERR-AMC-03: You are asigning more credits to a non existing category. (sum, staking)",
                            category.to_string(),
                            vector_to_string(stakeconfig.categories)
                        )
                    );
                }
                check(suma.amount <= staking.total_stake.amount, 
                    create_error_asset2(
                        "ERR-AMC-03: You are asigning more credits that you actually have tokens in stake. (sum, staking)",
                        suma,
                        staking.total_stake
                    )
                );

                int days                = vapaee::utils::check_integer_from_string(stakeconfig.credits_locktime);
                time_point_sec _now     = vapaee::dex::global::get_now_time_point_sec();
                time_point_sec _release = vapaee::dex::global::get_N_days_from_point_sec(staking.last_update, days);
                check(_now > _release, 
                    create_error_id1("ERR-AMC-04: you have to wait in order to change credits' categories. (days to wait): ", days).c_str()
                );

                // update tables
                staking.last_update  = _now;
                staking.credits      = formated_credits;
                
                // ------------------
                PRINT(" > suma: ", suma.to_string(), "\n");
                PRINT(" > staking.total_stake: ", staking.total_stake.to_string(), "\n");
                i=0; for(const auto& c: staking.credits) {
                    PRINT(" staking.credits[",i++,"] ", c.to_string(),  "\n");
                }
                // check(false, "WHAT?");
                // ------------------

                // if (
                //     suma.to_string() == string("0.0000 KOINE") &&
                //     owner == name("alice")
                // ) {
                //     check(false, "A VER, ALICE !!!!!");
                // }

                // save tables in RAM
                get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);

                PRINT("vapaee::pay::rex::action_mycredits() ...\n");
            }

            void action_unstake(
                const name& owner,
                const asset& quantity,
                const name& pool_id,
                const std::vector<std::tuple<name, asset>>& credits
            ) {
                PRINT("vapaee::pay::rex::action_unstake()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" pool_id: ", pool_id.to_string(), "\n");
                int i=0; for(const auto& c: credits) {
                    PRINT(" credits[",i++,"]: ", std::get<0>(c).to_string(), " - ", std::get<1>(c).to_string(),  "\n");
                }
                
                require_auth(owner);
                name ram_payer = owner;
                symbol_code token = quantity.symbol.code();

                // get staking configurations and state
                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-AUS-01");

                // get pool configurations and state
                stakepool_table stakepool;
                get_stakepool_for_pool_id(false, token, pool_id, stakepool, ram_payer, "ERR-AUS-02");

                // get staking entry
                staking_table staking;
                get_owner_staking_for_token(false, owner, token, staking, ram_payer, "ERR-AUS-03");

                // get poolstaking entry
                poolstaking_table poolstaking;
                get_owner_poolstaking_for_pool_id(false, owner, token, pool_id, poolstaking, ram_payer, "ERR-AUS-04");

                // calculate
                double rex_price       = stakepool.rex_price();
                asset stake_withdraw   = quantity;
                double percent         = (double) quantity.amount / (double) poolstaking.stake.amount;
                asset rex_withdraw     = asset(poolstaking.rex.amount * percent, stakepool.pool_rex.symbol);
                asset funds_withdraw   = asset(rex_withdraw.amount * rex_price, stakepool.pool_funds.symbol);  

                PRINT(" > rex_price: ", std::to_string(rex_price), "\n");
                PRINT(" > percent: ", std::to_string(percent), "\n");
                PRINT(" > stake_withdraw: ", stake_withdraw.to_string(), "\n");
                PRINT(" > rex_withdraw: ", rex_withdraw.to_string(), "\n");
                PRINT(" > funds_withdraw: ", funds_withdraw.to_string(), "\n");

                // check there's enough balance
                check(stakeconfig.total_stake >= stake_withdraw, create_error_asset2("ERR-AUS-05: Not enough balance: ", stakeconfig.total_stake,stake_withdraw).c_str());
                check(stakeconfig.total_funds >= funds_withdraw, create_error_asset2("ERR-AUS-06: Not enough balance: ", stakeconfig.total_funds,funds_withdraw).c_str());
                check(stakepool.pool_stake    >= stake_withdraw, create_error_asset2("ERR-AUS-07: Not enough balance: ", stakepool.pool_stake,stake_withdraw).c_str());
                check(stakepool.pool_funds    >= funds_withdraw, create_error_asset2("ERR-AUS-08: Not enough balance: ", stakepool.pool_funds,funds_withdraw).c_str());
                check(stakepool.pool_rex      >= rex_withdraw,   create_error_asset2("ERR-AUS-09: Not enough balance: ", stakepool.pool_rex,rex_withdraw).c_str());
                check(poolstaking.rex         >= rex_withdraw,   create_error_asset2("ERR-AUS-10: Not enough balance: ", poolstaking.rex,rex_withdraw).c_str());
                check(poolstaking.mature      >= stake_withdraw, create_error_asset2("ERR-AUS-11: Not enough mature balance: ", poolstaking.mature,stake_withdraw).c_str());
                check(poolstaking.stake       >= stake_withdraw, create_error_asset2("ERR-AUS-12: Not enough balance: ", poolstaking.stake,stake_withdraw).c_str());
                check(staking.total_stake     >= stake_withdraw, create_error_asset2("ERR-AUS-13: Not enough balance: ", staking.total_stake,stake_withdraw).c_str());
                check(staking.total_mature    >= stake_withdraw, create_error_asset2("ERR-AUS-14: Not enough mature balance: ", staking.total_mature,stake_withdraw).c_str());

                // update
                stakeconfig.total_stake -= stake_withdraw;
                stakeconfig.total_funds -= funds_withdraw;
                stakepool.pool_stake    -= stake_withdraw;
                stakepool.pool_funds    -= funds_withdraw;
                stakepool.pool_rex      -= rex_withdraw;
                poolstaking.rex         -= rex_withdraw;
                poolstaking.mature      -= stake_withdraw;
                poolstaking.stake       -= stake_withdraw;
                staking.total_stake     -= stake_withdraw;
                staking.total_mature    -= stake_withdraw;
                staking.last_update      = time_point_sec::min(); // this avoid action_mycredits to cancel because locktime

                // save tables in RAM
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);
                get_stakepool_for_pool_id(true, token, pool_id, stakepool, ram_payer, NULL);
                get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);
                get_owner_poolstaking_for_pool_id(true, owner, token, pool_id, poolstaking, ram_payer, NULL);

                action_mycredits(owner, credits);

                string memo = string("Unstaking from staking pool: ") + token.to_string() + "-" + pool_id.to_string();
                send_transfer_tokens(get_self(), owner, funds_withdraw, memo, vapaee::token::contract);

                PRINT("vapaee::pay::rex::action_unstake() ...\n");
            }

            // take proits without touching the stake
            void action_takeprofits(
                const name& owner,
                const symbol_code& token,
                const name& pool_id
            ) {
                PRINT("vapaee::pay::rex::action_unstake()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" pool_id: ", pool_id.to_string(), "\n");

                require_auth(owner);
                name ram_payer = owner;

                // get staking configurations and state
                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-ATP-01");

                // get pool configurations and state
                stakepool_table stakepool;
                get_stakepool_for_pool_id(false, token, pool_id, stakepool, ram_payer, "ERR-ATP-02");

                // get poolstaking entry
                poolstaking_table poolstaking;
                get_owner_poolstaking_for_pool_id(false, owner, token, pool_id, poolstaking, ram_payer, "ERR-ATP-04");

                // calculate
                double rex_price       = stakepool.rex_price();
                double rex_inverse     = stakepool.rex_inverse();
                asset rex              = asset(poolstaking.stake.amount * rex_inverse, stakepool.pool_rex.symbol); // esto es la cantidad de rex que debería tener al precio actual, pero tiene más
                asset rex_withdraw     = poolstaking.rex - rex; // la diferencia es lo que se retira
                asset funds_withdraw   = asset(rex_withdraw.amount * rex_price, stakepool.pool_funds.symbol);  
                
                PRINT(" > rex_price: ", std::to_string(rex_price), "\n");
                PRINT(" > rex_inverse: ", std::to_string(rex_inverse), "\n");
                PRINT(" > rex: ", rex.to_string(), "\n");
                PRINT(" > rex_withdraw: ", rex_withdraw.to_string(), "\n");
                PRINT(" > funds_withdraw: ", funds_withdraw.to_string(), "\n");

                // check there's enough balance
                check(stakeconfig.total_funds >= funds_withdraw, create_error_asset2("ERR-ATP-05: not enough balance: ", stakeconfig.total_funds, funds_withdraw).c_str());
                check(stakepool.pool_funds    >= funds_withdraw, create_error_asset2("ERR-ATP-06: not enough balance: ", stakepool.pool_funds, funds_withdraw).c_str());
                check(stakepool.pool_rex      >= rex_withdraw,   create_error_asset2("ERR-ATP-07: not enough balance: ", stakepool.pool_rex, rex_withdraw).c_str());
                check(poolstaking.rex         >= rex_withdraw,   create_error_asset2("ERR-ATP-08: not enough balance: ", poolstaking.rex, rex_withdraw).c_str());

                // update tables
                stakeconfig.total_funds -= funds_withdraw;
                stakepool.pool_funds    -= funds_withdraw;
                stakepool.pool_rex      -= rex_withdraw;
                poolstaking.rex         -= rex_withdraw;

                // save tables in RAM
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);
                get_stakepool_for_pool_id(true, token, pool_id, stakepool, ram_payer, NULL);
                get_owner_poolstaking_for_pool_id(true, owner, token, pool_id, poolstaking, ram_payer, NULL);

                string memo = string("Taking profits from staking pool: ") + token.to_string() + "-" + pool_id.to_string();
                send_transfer_tokens(get_self(), owner, funds_withdraw, memo, vapaee::token::contract);

                PRINT("vapaee::pay::rex::action_unstake() ...\n");
            }

            // update maturing funds
            void action_updtstake(
                const name& owner,
                const symbol_code& token,
                const name& pool_id
            ) {
                PRINT("vapaee::pay::rex::action_updtstake()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" pool_id: ", pool_id.to_string(), "\n");

                // require admin owner or contract
                name ram_payer = owner;
                if (has_auth(get_self())) {
                    ram_payer = get_self();
                } else {
                    require_auth(owner);
                }

                // get staking entry
                staking_table staking;
                get_owner_staking_for_token(false, owner, token, staking, ram_payer, "ERR-AUS-03");

                // get poolstaking entry
                poolstaking_table poolstaking;
                get_owner_poolstaking_for_pool_id(false, owner, token, pool_id, poolstaking, ram_payer, "ERR-AUS-04");

                // find matured stake
                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                asset mature = asset(0, poolstaking.mature.symbol);
                std::vector<maturing_funds> maturing;
                PRINT(" > _now:    ", std::to_string((unsigned)_now.utc_seconds), "\n");

                for(int i=0; i<poolstaking.maturing.size(); i++) {
                    PRINT(" > poolstaking.maturing[",std::to_string(i),"].mature: ", std::to_string((unsigned)poolstaking.maturing[i].mature.utc_seconds), "\n");
                    if (poolstaking.maturing[i].mature <= _now) {
                        mature += poolstaking.maturing[i].stake;
                    } else {
                        maturing.push_back(poolstaking.maturing[i]);
                    }
                }

                // update tables
                poolstaking.maturing   = maturing;
                poolstaking.mature    += mature;
                staking.total_mature  += mature;

                // save tables in RAM
                get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);
                get_owner_poolstaking_for_pool_id(true, owner, token, pool_id, poolstaking, ram_payer, NULL);


                check(mature.amount > 0, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

                PRINT("vapaee::pay::rex::action_updtstake() ...\n");
            }
        };     
    };
};
