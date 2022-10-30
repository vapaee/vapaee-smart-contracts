#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/tables.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/token/modules/standard.hpp>
#include <vapaee/token/modules/debit.hpp>
#include <vapaee/token/modules/wrapper.hpp>
#include <vapaee/token/modules/utils.hpp>

#define MAX_STAKING_HISTORY 6
#define HISTORY_OFFSET 3600
#define USE_FILTER false

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

                std::vector<history_entry> history;
                for (int i=0; i<config.history.size(); i++)
                    history.push_back(config.history[i]);

                stakeconfigs stkconf_table(get_self(), get_self().value);
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
                        a.categories        = categories;
                        a.history           = history;
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
                        a.categories        = categories;
                        a.history           = history;
                        a.credits_locktime  = config.credits_locktime;
                        a.total_stake       = config.total_stake;
                        a.total_funds       = config.total_funds;                                                
                    });

                    return true;
                }
                
                config.token = config_ptr->token;
                config.admin = config_ptr->admin;
                config.title = config_ptr->title;

                config.categories.clear();
                for (int i=0; i<config_ptr->categories.size(); i++)
                    config.categories.push_back(config_ptr->categories[i]);

                config.history.clear();
                for (int i=0; i<config_ptr->history.size(); i++)
                    config.history.push_back(config_ptr->history[i]);
                
                config.credits_locktime = config_ptr->credits_locktime;
                config.total_stake = config_ptr->total_stake;
                config.total_funds = config_ptr->total_funds;

                return true;
            }

            
            bool get_stakepool_for_pool_id(
                bool create,
                const symbol_code& token,
                const name& label,
                stakepool_table& stakepool,
                const name& ram_payer,
                const char * error_code
            ) {
                PRINT("vapaee::pay::rex::get_stakepool_for_pool_id()\n");
                PRINT(" create: ", std::to_string(create), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" label: ", label.to_string(), "\n");
                PRINT(" stakepool.pool: ", stakepool.pool.to_string(), "\n");

                stakepools stkpool_table(get_self(), token.raw());
                auto pool_ptr = stkpool_table.find(label.value);

                if (pool_ptr == stkpool_table.end() && !create)  {
                    if (error_code) {
                        check(false, 
                            create_error_string2(
                                string(string(error_code) + ": the stakepools entry for the token and label was not found. (token, label): ").c_str(),
                                token.to_string(),
                                label.to_string()
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (pool_ptr == stkpool_table.end() && create) {
                    stkpool_table.emplace(ram_payer, [&](auto &a){ 
                        a.id.label   = label;
                        a.id.token   = token;
                        a.locktime   = stakepool.locktime;
                        a.title      = stakepool.title;
                        a.pool_stake = stakepool.pool_stake;
                        a.pool_funds = stakepool.pool_funds;
                        a.pool_rex   = stakepool.pool_rex;
                        a.history.clear();
                        for (int i=0; i<stakepool.history.size(); i++) {
                            a.history.push_back(stakepool.history[i]);
                        }
                    });

                    return true;
                };

                if (pool_ptr != stkpool_table.end() && create) {
                    stkpool_table.modify(pool_ptr, ram_payer, [&](auto &a){
                        a.locktime   = stakepool.locktime;
                        a.title      = stakepool.title;
                        a.pool_stake = stakepool.pool_stake;
                        a.pool_funds = stakepool.pool_funds;
                        a.pool_rex   = stakepool.pool_rex;
                        a.history.clear();
                        for (int i=0; i<stakepool.history.size(); i++) {
                            a.history.push_back(stakepool.history[i]);
                        }
                    });

                    return true;
                };
                
                stakepool.id.label        = pool_ptr->id.label;
                stakepool.id.token        = pool_ptr->id.token;

                stakepool.locktime        = pool_ptr->locktime;
                stakepool.title           = pool_ptr->title;

                stakepool.pool_stake      = pool_ptr->pool_stake;
                stakepool.pool_funds      = pool_ptr->pool_funds;
                stakepool.pool_rex        = pool_ptr->pool_rex;

                stakepool.history.clear();
                for (int i=0; i<pool_ptr->history.size(); i++) {
                    stakepool.history.push_back(pool_ptr->history[i]);
                }

                return true;
            }

            
            bool get_owner_staking_for_token(
                bool create,
                const name& owner,
                const symbol_code& token,
                mystake_table& stake,
                const name& ram_payer,
                const char * error_code
            ) {

                mystake mystake_table(get_self(), owner.value);
                auto stake_ptr = mystake_table.find(token.raw());

                if (stake_ptr == mystake_table.end() && !create) {
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

                if (stake_ptr == mystake_table.end() && create) {
                    mystake_table.emplace(ram_payer, [&](auto &a){
                        a.total_stake     = stake.total_stake;
                        a.total_mature    = stake.total_mature;
                        a.credits_update  = stake.credits_update;
                        a.credits.clear();
                        for (int i=0; i<stake.credits.size(); i++)
                            a.credits.push_back(stake.credits[i]);
                    });

                    return true;
                }

                if (stake_ptr != mystake_table.end() && create) {
                    mystake_table.modify(stake_ptr, ram_payer, [&](auto &a) {
                        a.total_stake     = stake.total_stake;
                        a.total_mature    = stake.total_mature;
                        a.credits_update  = stake.credits_update;
                        a.credits.clear();
                        for (int i=0; i<stake.credits.size(); i++)
                            a.credits.push_back(stake.credits[i]);
                    });
                }
                
                stake.total_stake     = stake_ptr->total_stake;
                stake.total_mature    = stake_ptr->total_mature;
                stake.credits_update  = stake_ptr->credits_update;

                stake.credits.clear();
                for (int i=0; i<stake_ptr->credits.size(); i++)
                    stake.credits.push_back(stake_ptr->credits[i]);


                return true;
            }

                        
            bool get_owner_mypoolstake_for_pool_id(
                bool create,
                const name& owner,
                const symbol_code& token,
                const name& poollabel,
                mypoolstake_table& pool,
                const name& ram_payer,
                const char * error_code
            ) {

                mypoolstake mypoolstake_table(get_self(), owner.value);
                auto index = mypoolstake_table.get_index<name("poolstake")>(); 
                auto pool_ptr = index.find(
                    vapaee::utils::pack( token.raw(), poollabel.value )
                );

                

                if (pool_ptr == index.end() && !create) {
                    if (error_code) {
                        check(false, 
                            create_error_string3(
                                string(string(error_code) + ": the owner's mypoolstake entry for the token and poollabel was not found. (owner, token, poollabel): ").c_str(),
                                owner.to_string(),
                                token.to_string(),
                                poollabel.to_string()
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (pool_ptr == index.end() && create) {
                    mypoolstake_table.emplace(ram_payer, [&](auto &a){
                        a.id           = mypoolstake_table.available_primary_key();
                        a.poollabel    = poollabel;
                        a.stake        = pool.stake;
                        a.mature       = pool.mature;
                        a.rex          = pool.rex;
                        a.stake_update = pool.stake_update;
                        a.maturing.clear();
                        for (int i=0; i<pool.maturing.size(); i++) {
                            a.maturing.push_back(pool.maturing[i]);
                        }
                    });

                    return true;
                }

                if (pool_ptr != index.end() && create) {
                    index.modify(pool_ptr, ram_payer, [&](auto &a){
                        a.stake        = pool.stake;
                        a.mature       = pool.mature;
                        a.rex          = pool.rex;
                        a.stake_update = pool.stake_update;
                        a.maturing.clear();
                        for (int i=0; i<pool.maturing.size(); i++) {
                            a.maturing.push_back(pool.maturing[i]);
                        }
                    });

                    return true;
                }

                pool.id           = pool_ptr->id;
                pool.poollabel    = pool_ptr->poollabel;
                pool.stake        = pool_ptr->stake;
                pool.rex          = pool_ptr->rex;
                pool.mature       = pool_ptr->mature;
                pool.stake_update = pool_ptr->stake_update;

                for (int i=0; i<pool_ptr->maturing.size(); i++) {
                    pool.maturing.push_back(pool_ptr->maturing[i]);
                }
                return true;
            }

            // ----- staking APR & recent history ------

            void update_stakepool_recent_history(stakepool_table& stakepool, const asset& quantity) {
                PRINT("vapaee::pay::rex::update_stakepool_recent_history()\n");
                PRINT(" stakepool.history.size(): ", stakepool.history.size(), "\n");
                for(int j=0; j<stakepool.history.size(); j++) {
                    PRINT(" stakepool.history[",j,"]: ", stakepool.history[j].to_string(), "\n");
                }
                PRINT(" quantity: ", quantity.to_string(), "\n");
                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                history_entry snap;
                snap.date = _now;
                snap.quantity = quantity;

                if (stakepool.history.size() <= 1) {
                    // no history yet, just add the new one
                    stakepool.history.push_back(snap);
                } else {
                    // if the last one's date is younger than 1 min, just update it
                    uint32_t offset = HISTORY_OFFSET; // 1h
                    AUX_DEBUG_CODE(offset = 10;);

                    uint32_t last = stakepool.history[stakepool.history.size()-1].date.utc_seconds;
                    uint32_t penultimate = stakepool.history[stakepool.history.size()-2].date.utc_seconds;

                    PRINT(" last: ", std::to_string((unsigned long) last ), "\n");
                    PRINT(" penultimate: ", std::to_string((unsigned long) penultimate ), "\n");
                    
                    if (last == _now.utc_seconds) {
                        PRINT(" if (_last == _now) --> true \n");
                        stakepool.history.back().quantity += quantity;                        
                    } else {
                        if ( last < penultimate + offset ) {
                            PRINT(" if ( last < penultimate + offset ) --> true \n");     
                            stakepool.history.back().date = _now;
                            stakepool.history.back().quantity += quantity;
                        } else {
                            // if the last one's date is older than 1 hour, just add the new one
                            PRINT(" if ( last < penultimate + offset ) --> false \n");    
                            stakepool.history.push_back(snap);
                        }
                    }

                }

                if (USE_FILTER) {
                    // filter vertor to remove entries not fiting the following criteria:
                    // the difference between the previous entry and the current one sould be less than half
                    // of the difference between the current one and the next one
                    vector<history_entry> filtered;
                    for (int i = stakepool.history.size() - 1; i >= 0; i--) {
                        if (i < 1 || i >= stakepool.history.size() - 2) {
                            filtered.push_back(stakepool.history[i]);
                        } else {                               
                            uint32_t prev_diff = stakepool.history[i+1].date.utc_seconds - stakepool.history[i].date.utc_seconds;
                            uint32_t curr_diff = stakepool.history[i].date.utc_seconds - stakepool.history[i-1].date.utc_seconds;
                            if (prev_diff <= curr_diff/2) {
                                filtered.push_back(stakepool.history[i]);
                            } else {
                                filtered.back().quantity += stakepool.history[i].quantity;
                            }
                        }
                    }
                    stakepool.history.clear();
                    for (int i = filtered.size() - 1; i >= 0; i--) {
                        stakepool.history.push_back(filtered[i]);
                    }
                }

                // if list is too long, remove the oldest one
                if (stakepool.history.size() > MAX_STAKING_HISTORY) {
                    stakepool.history.erase(stakepool.history.begin());
                }
                
            }

            void update_stakeconfig_recent_history(stakeconfigs_table& stakeconfig, const asset& quantity) {
                PRINT("vapaee::pay::rex::update_stakeconfig_recent_history()\n");
                PRINT(" stakeconfig.history.size(): ", stakeconfig.history.size(), "\n");
                for(int j=0; j<stakeconfig.history.size(); j++) {
                    PRINT(" stakeconfig.history[",j,"]: ", stakeconfig.history[j].to_string(), "\n");
                }
                
                history_entry snap;
                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                snap.date = _now;
                snap.quantity = quantity;
               
                PRINT(" _now: ", std::to_string((unsigned long) _now.utc_seconds ), "\n");

                if (stakeconfig.history.size() <= 1) {
                    // no history yet, just add the new one
                    stakeconfig.history.push_back(snap);
                } else {
                    // if the last one's date is younger than 1 min, just update it
                    uint32_t offset = HISTORY_OFFSET; // 1h
                    AUX_DEBUG_CODE(offset = 10;);

                    uint32_t last = stakeconfig.history[stakeconfig.history.size()-1].date.utc_seconds;
                    uint32_t penultimate = stakeconfig.history[stakeconfig.history.size()-2].date.utc_seconds;

                    PRINT(" last: ", std::to_string((unsigned long) last ), "\n");
                    PRINT(" penultimate: ", std::to_string((unsigned long) penultimate ), "\n");

                    if (last == _now.utc_seconds) {
                        PRINT(" if (_last == _now) --> true \n");
                        stakeconfig.history.back().quantity += quantity;                        
                    } else {
                        if ( last < penultimate + offset ) {
                            PRINT(" if ( last < penultimate + offset ) --> true \n");     
                            stakeconfig.history.back().date = _now;
                            stakeconfig.history.back().quantity += quantity;
                        } else {
                            // if the last one's date is older than 1 hour, just add the new one
                            PRINT(" if ( last < penultimate + offset ) --> false \n");    
                            stakeconfig.history.push_back(snap);
                        }
                    }
                }

                if (USE_FILTER) {
                    // filter vertor to remove entries not fiting the following criteria:
                    // the difference between the previous entry and the current one sould be less than half
                    // of the difference between the current one and the next one
                    vector<history_entry> filtered;
                    for (uint32_t i = 0; i < stakeconfig.history.size(); i++) {
                        if (i == 0 || i == stakeconfig.history.size() - 1) {
                            // first and last entries are always included
                            filtered.push_back(stakeconfig.history[i]);
                        } else {
                            uint32_t prev_diff = stakeconfig.history[i+1].date.utc_seconds - stakeconfig.history[i].date.utc_seconds;
                            uint32_t curr_diff = stakeconfig.history[i].date.utc_seconds - stakeconfig.history[i-1].date.utc_seconds;
                            if (prev_diff <= curr_diff/2) {
                                filtered.push_back(stakeconfig.history[i]);
                            } else {
                                filtered.back().quantity += stakeconfig.history[i].quantity;
                            }
                        }
                    }
                    stakeconfig.history.clear();
                    for (int i = filtered.size() - 1; i >= 0; i--) {
                        stakeconfig.history.push_back(filtered[i]);
                    }
                }

                // if list is too long, remove the oldest one
                if (stakeconfig.history.size() > MAX_STAKING_HISTORY) {
                    stakeconfig.history.erase(stakeconfig.history.begin());
                }


                if (stakeconfig.history.size() > 2) {
                        
                    uint32_t last = stakeconfig.history[stakeconfig.history.size()-1].date.utc_seconds;
                    uint32_t penultimate = stakeconfig.history[stakeconfig.history.size()-2].date.utc_seconds;

                    if (last == penultimate) {
                        PRINT(" stakeconfig.history.size(): ", stakeconfig.history.size(), "\n");
                        for(int j=0; j<stakeconfig.history.size(); j++) {
                            PRINT(" stakeconfig.history[",j,"]: ", stakeconfig.history[j].to_string(), "\n");
                        }

                        check(false, "BOOMB");
                    }
                }

                
            }
          
            // actions ---

            // admin registrates a token for staking or updates staking info
            void action_stakeconfig(
                const name& action,
                const name& admin,
                const symbol_code& token,
                const name& token_contract,
                const string& title,
                const std::vector<name>& categories,
                const string& credits_locktime
            ) {
                PRINT("vapaee::pay::rex::action_stakeconfig()\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" token_contract: ", token_contract.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
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
                const name& poollabel,
                const string& title,
                const string& locktime
            ) {
                PRINT("vapaee::pay::rex::action_stakepool()\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" poollabel: ", poollabel.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" locktime: ", locktime.c_str(), "\n");

                // Get the existing (if any) pointer to the staking confituration

                stakeconfigs_table config;
                get_stakeconfig_for_token(false, token, config, get_self(), "ERR-ASP-1");

                // require admin auth or contract
                name ram_payer = config.admin;
                if (has_auth(get_self())) {
                    ram_payer = get_self();
                } else {
                    require_auth(config.admin);
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
                stakepool.id.token   = token;
                stakepool.id.label   = poollabel;
                stakepool.locktime   = locktime;
                stakepool.title      = title;
                stakepool.pool_stake = zero_token;
                stakepool.pool_funds = zero_token;
                stakepool.pool_rex   = zero_rex;
                // update_stakepool_recent_history(stakepool);
                get_stakepool_for_pool_id(true, token, poollabel, stakepool, ram_payer, "ERR-ASP-2");
                
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
                const name& poollabel
            ) {
                PRINT("vapaee::pay::rex::action_droponpool()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" poollabel: ", poollabel.to_string(), "\n");

                require_auth(get_self());
                name ram_payer = get_self();
                symbol_code token = quantity.symbol.code();

                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-ADS-01");

                stakepool_table stakepool;
                get_stakepool_for_pool_id(false, token, poollabel, stakepool, ram_payer, "ERR-ADS-02");

                stakeconfig.total_funds += quantity;
                stakepool.pool_funds += quantity;
                
                // update recent history
                update_stakepool_recent_history(stakepool, quantity);
                update_stakeconfig_recent_history(stakeconfig, quantity);

                check(stakepool.history.size() > 0, "ERR-ADS-03: stakepool.history is empty");

                get_stakepool_for_pool_id(true, token, poollabel, stakepool, ram_payer, NULL);
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);

                PRINT("vapaee::pay::rex::action_droponpool() ...\n");
            }

            void action_stake(
                const name& owner,
                const asset& quantity,
                const name& poollabel
            ) {
                PRINT("vapaee::pay::rex::action_stake()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" poollabel: ", poollabel.to_string(), "\n");

                require_auth(owner);
                name ram_payer = owner;
                symbol_code token = quantity.symbol.code();
                asset supply = vapaee::token::wrapper::get_token_supply(token, 
                            create_error_symcode1("ERR-AS-01: could not get supply of the token:", token));
                asset zero_token = asset(0, supply.symbol);
                asset zero_rex = asset(0, symbol(vapaee::pay::REX_TKN_CODE, supply.symbol.precision()));

                // get or create staking entry
                mystake_table staking;
                bool exists = get_owner_staking(false, owner, token, staking, ram_payer, NULL);
                if (!exists) {
                    staking.total_stake        = zero_token;
                    staking.total_mature       = zero_token;
                    staking.credits_update     = time_point_sec(0);
                    get_owner_staking(true, owner, token, staking, ram_payer, NULL);  
                }

                // get or create mypoolstake entry
                mypoolstake_table mypstake;
                exists = get_owner_mypoolstake(false, owner, token, poollabel, mypstake, ram_payer, NULL);
                if (!exists) {
                    mypstake.poollabel    = poollabel;
                    mypstake.stake        = zero_token;
                    mypstake.mature       = zero_token;
                    mypstake.rex          = zero_rex;
                    mypstake.stake_update = vapaee::dex::global::get_now_time_point_sec();
                    get_owner_mypoolstake_for_pool_id(true, owner, token, poollabel, mypstake, ram_payer, NULL);
                }

                // get staking and pool configurations and state
                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-AS-02");

                stakepool_table stakepool;
                get_stakepool_for_pool_id(false, token, poollabel, stakepool, ram_payer, "ERR-AS-03");

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
                mypstake.rex            += rex_deposit;
                mypstake.stake          += stake_deposit;
                mypstake.stake_update    = vapaee::dex::global::get_now_time_point_sec();
                maturing_funds maturing;
                maturing.stake           = stake_deposit;
                maturing.mature          = mature;
                mypstake.maturing.push_back(maturing);

                
                // update recent history
                // update_stakepool_recent_history(stakepool);
                // update_stakeconfig_recent_history(stakeconfig);

                // save tables in RAM
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);
                get_stakepool_for_pool_id(true, token, poollabel, stakepool, ram_payer, NULL);
                get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);
                get_owner_mypoolstake_for_pool_id(true, owner, token, poollabel, mypstake, ram_payer, NULL);

                // debit quantity from owner
                string memo = string("staking ") + quantity.to_string() + " into " + token.to_string() + "-" + poollabel.to_string() + " pool.";
                name contract = vapaee::dex::utils::get_contract_for_token(supply.symbol.code());
                vapaee::token::utils::send_debit_to_owner(owner, get_self(), quantity, memo);

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
                mystake_table staking;
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
                time_point_sec _release = vapaee::dex::global::get_N_days_from_point_sec(staking.credits_update, days);
                check(_now > _release, 
                    create_error_id1("ERR-AMC-04: you have to wait in order to change credits' categories. (days to wait): ", days).c_str()
                );

                // update tables
                staking.credits_update  = _now;
                staking.credits         = formated_credits;
                
                // ------------------
                PRINT(" > suma: ", suma.to_string(), "\n");
                PRINT(" > staking.total_stake: ", staking.total_stake.to_string(), "\n");
                i=0; for(const auto& c: staking.credits) {
                    PRINT(" staking.credits[",i++,"] ", c.to_string(),  "\n");
                }
                // check(false, "WHAT?");
                // ------------------

                // if (
                //     suma.to_string() == string("0.000000 KOINE") &&
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
                const name& poollabel,
                const std::vector<std::tuple<name, asset>>& credits
            ) {
                PRINT("vapaee::pay::rex::action_unstake()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" poollabel: ", poollabel.to_string(), "\n");
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
                get_stakepool_for_pool_id(false, token, poollabel, stakepool, ram_payer, "ERR-AUS-02");

                // get staking entry
                mystake_table staking;
                get_owner_staking_for_token(false, owner, token, staking, ram_payer, "ERR-AUS-03");

                // get mypoolstake entry
                mypoolstake_table mypstake;
                get_owner_mypoolstake_for_pool_id(false, owner, token, poollabel, mypstake, ram_payer, "ERR-AUS-04");

                // calculate
                double rex_price       = stakepool.rex_price();
                asset stake_withdraw   = quantity;
                double percent         = (double) quantity.amount / (double) mypstake.stake.amount;
                asset rex_withdraw     = asset(mypstake.rex.amount * percent, stakepool.pool_rex.symbol);
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
                check(mypstake.rex            >= rex_withdraw,   create_error_asset2("ERR-AUS-10: Not enough balance: ", mypstake.rex,rex_withdraw).c_str());
                check(mypstake.mature         >= stake_withdraw, create_error_asset2("ERR-AUS-11: Not enough mature balance: ", mypstake.mature,stake_withdraw).c_str());
                check(mypstake.stake          >= stake_withdraw, create_error_asset2("ERR-AUS-12: Not enough balance: ", mypstake.stake,stake_withdraw).c_str());
                check(staking.total_stake     >= stake_withdraw, create_error_asset2("ERR-AUS-13: Not enough balance: ", staking.total_stake,stake_withdraw).c_str());
                check(staking.total_mature    >= stake_withdraw, create_error_asset2("ERR-AUS-14: Not enough mature balance: ", staking.total_mature,stake_withdraw).c_str());

                // update
                stakeconfig.total_stake -= stake_withdraw;
                stakeconfig.total_funds -= funds_withdraw;
                stakepool.pool_stake    -= stake_withdraw;
                stakepool.pool_funds    -= funds_withdraw;
                stakepool.pool_rex      -= rex_withdraw;
                mypstake.rex            -= rex_withdraw;
                mypstake.mature         -= stake_withdraw;
                mypstake.stake          -= stake_withdraw;
                mypstake.stake_update    = vapaee::dex::global::get_now_time_point_sec();
                staking.total_stake     -= stake_withdraw;
                staking.total_mature    -= stake_withdraw;
                

                if (credits.size() > 0) {
                    // this avoid action_mycredits to cancel because locktime
                    staking.credits_update = time_point_sec::min(); 
                }

                // update recent history
                // update_stakepool_recent_history(stakepool);
                // update_stakeconfig_recent_history(stakeconfig);

                // save tables in RAM
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);
                get_stakepool_for_pool_id(true, token, poollabel, stakepool, ram_payer, NULL);
                get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);
                get_owner_mypoolstake_for_pool_id(true, owner, token, poollabel, mypstake, ram_payer, NULL);

                if (credits.size() > 0) {
                    action_mycredits(owner, credits);
                }

                string memo = string("Unstaking from staking pool: ") + token.to_string() + "-" + poollabel.to_string();
                name contract = vapaee::dex::utils::get_contract_for_token(funds_withdraw.symbol.code());
                vapaee::token::utils::send_transfer_tokens(get_self(), owner, funds_withdraw, memo, contract);

                PRINT("vapaee::pay::rex::action_unstake() ...\n");
            }

            // take proits without touching the stake
            void action_takeprofits(
                const name& owner,
                const symbol_code& token,
                const name& poollabel
            ) {
                PRINT("vapaee::pay::rex::action_takeprofits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" poollabel: ", poollabel.to_string(), "\n");

                require_auth(owner);
                name ram_payer = owner;

                // get staking configurations and state
                stakeconfigs_table stakeconfig;
                get_stakeconfig_for_token(false, token, stakeconfig, ram_payer, "ERR-ATP-01");

                // get pool configurations and state
                stakepool_table stakepool;
                get_stakepool_for_pool_id(false, token, poollabel, stakepool, ram_payer, "ERR-ATP-02");

                // get mypoolstake entry
                mypoolstake_table mypstake;
                get_owner_mypoolstake_for_pool_id(false, owner, token, poollabel, mypstake, ram_payer, "ERR-ATP-04");

                // calculate
                double rex_price       = stakepool.rex_price();
                double rex_inverse     = stakepool.rex_inverse();
                asset rex              = asset(mypstake.stake.amount * rex_inverse, stakepool.pool_rex.symbol); // esto es la cantidad de rex que debería tener al precio actual, pero tiene más
                asset rex_withdraw     = mypstake.rex - rex; // la diferencia es lo que se retira
                asset funds_withdraw   = asset(rex_withdraw.amount * rex_price, stakepool.pool_funds.symbol);  
                
                PRINT(" > rex_price: ", std::to_string(rex_price), "\n");
                PRINT(" > rex_inverse: ", std::to_string(rex_inverse), "\n");
                PRINT(" > rex: ", rex.to_string(), "\n");
                PRINT(" > rex_withdraw: ", rex_withdraw.to_string(), "\n");
                PRINT(" > funds_withdraw: ", funds_withdraw.to_string(), "\n");

                // check there's enough balance
                check(funds_withdraw.amount > 0, "ERR-ATP-05: there's no profits to withdraw.");
                check(stakeconfig.total_funds >= funds_withdraw, create_error_asset2("ERR-ATP-06: not enough balance: ", stakeconfig.total_funds, funds_withdraw).c_str());
                check(stakepool.pool_funds    >= funds_withdraw, create_error_asset2("ERR-ATP-07: not enough balance: ", stakepool.pool_funds, funds_withdraw).c_str());
                check(stakepool.pool_rex      >= rex_withdraw,   create_error_asset2("ERR-ATP-08: not enough balance: ", stakepool.pool_rex, rex_withdraw).c_str());
                check(mypstake.rex            >= rex_withdraw,   create_error_asset2("ERR-ATP-09: not enough balance: ", mypstake.rex, rex_withdraw).c_str());

                // update tables
                stakeconfig.total_funds -= funds_withdraw;
                stakepool.pool_funds    -= funds_withdraw;
                stakepool.pool_rex      -= rex_withdraw;
                mypstake.rex            -= rex_withdraw;
                
                // update recent history
                // update_stakepool_recent_history(stakepool);
                // update_stakeconfig_recent_history(stakeconfig);

                // save tables in RAM
                get_stakeconfig_for_token(true, token, stakeconfig, ram_payer, NULL);
                get_stakepool_for_pool_id(true, token, poollabel, stakepool, ram_payer, NULL);
                get_owner_mypoolstake_for_pool_id(true, owner, token, poollabel, mypstake, ram_payer, NULL);

                string memo = string("Taking profits ("+rex_withdraw.to_string()+") from staking pool: ") + token.to_string() + "-" + pool_id.to_string();
                name contract = vapaee::dex::utils::get_contract_for_token(funds_withdraw.symbol.code());
                vapaee::token::utils::send_transfer_tokens(get_self(), owner, funds_withdraw, memo, contract);

            }

            // update maturing funds
            void action_updtstake(
                const name& owner,
                const symbol_code& token,
                const name& poollabel
            ) {
                PRINT("vapaee::pay::rex::action_updtstake()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" poollabel: ", poollabel.to_string(), "\n");

                // require admin owner or contract
                name ram_payer = owner;
                if (has_auth(get_self())) {
                    ram_payer = get_self();
                } else {
                    require_auth(owner);
                }

                // get staking entry
                mystake_table staking;
                get_owner_staking_for_token(false, owner, token, staking, ram_payer, "ERR-AUS-03");

                // get mypoolstake entry
                mypoolstake_table mypstake;
                get_owner_mypoolstake_for_pool_id(false, owner, token, poollabel, mypstake, ram_payer, "ERR-AUS-04");

                // find matured stake
                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                asset mature = asset(0, mypstake.mature.symbol);
                std::vector<maturing_funds> maturing;
                PRINT(" > _now:    ", std::to_string((unsigned)_now.utc_seconds), "\n");

                for(int i=0; i<mypstake.maturing.size(); i++) {
                    PRINT(" > mypstake.maturing[",std::to_string(i),"].mature: ", std::to_string((unsigned)mypstake.maturing[i].mature.utc_seconds), "\n");
                    if (mypstake.maturing[i].mature <= _now) {
                        mature += mypstake.maturing[i].stake;
                    } else {
                        maturing.push_back(mypstake.maturing[i]);
                    }
                }

                // update tables
                mypstake.maturing      = maturing;
                mypstake.mature       += mature;
                staking.total_mature  += mature;

                // save tables in RAM
                get_owner_staking_for_token(true, owner, token, staking, ram_payer, NULL);
                get_owner_mypoolstake_for_pool_id(true, owner, token, poollabel, mypstake, ram_payer, NULL);

                PRINT("vapaee::pay::rex::action_updtstake() ...\n");
            }
        };     
    };
};
