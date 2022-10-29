#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/book/modules/deposit.hpp>
#include <vapaee/dex/modules/fees.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/security.hpp>

namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace dao {

            const name decide = name("telos.decide");
            const name saving = name("eosio.saving");
            const symbol treasury_symbol = symbol(symbol_code("VOTE"), 4);

            // telos.decide table structs ------------------
            // state config config config config config config config
            // ----------------------------------------------------- 
            struct decide_config {
                string app_name;
                string app_version;
                asset total_deposits;
                map<name, asset> fees;     //ballot, treasury, archival
                map<name, uint32_t> times; //balcooldown, minballength, forfeittime

                uint64_t primary_key() const { return 0; }
            };

            typedef eosio::multi_index< "config"_n, decide_config > config_table;
            // typedef eosio::multi_index< "stat"_n, currency_stats > stats;
            // typedef singleton<name("tdconfig"), decide_config> decide_config_singleton;

            struct ballot {
                name ballot_name;
                name category; //proposal, referendum, election, poll, leaderboard
                name publisher;
                name status; //setup, voting, closed, cancelled, archived

                string title; //markdown
                string description; //markdown
                string content; //IPFS link to content or markdown

                symbol treasury_symbol; //treasury used for counting votes
                name voting_method; //1acct1vote, 1tokennvote, 1token1vote, 1tsquare1v, quadratic
                uint8_t min_options; //minimum options per voter
                uint8_t max_options; //maximum options per voter
                map<name, asset> options; //option name -> total weighted votes

                uint32_t total_voters; //number of voters who voted on ballot
                uint32_t total_delegates; //number of delegates who voted on ballot
                asset total_raw_weight; //total raw weight cast on ballot
                uint32_t cleaned_count; //number of expired vote receipts cleaned
                map<name, bool> settings; //setting name -> on/off
                
                time_point_sec begin_time; //time that voting begins
                time_point_sec end_time; //time that voting closes

                uint64_t primary_key() const { return ballot_name.value; }
                uint64_t by_category() const { return category.value; }
                uint64_t by_status() const { return status.value; }
                uint64_t by_symbol() const { return treasury_symbol.code().raw(); }
                uint64_t by_end_time() const { return static_cast<uint64_t>(end_time.utc_seconds); }
                
                EOSLIB_SERIALIZE(ballot, 
                    (ballot_name)(category)(publisher)(status)
                    (title)(description)(content)
                    (treasury_symbol)(voting_method)(min_options)(max_options)(options)
                    (total_voters)(total_delegates)(total_raw_weight)(cleaned_count)(settings)
                    (begin_time)(end_time))
            };
            typedef multi_index<name("ballots"), ballot,
                indexed_by<name("bycategory"), const_mem_fun<ballot, uint64_t, &ballot::by_category>>,
                indexed_by<name("bystatus"), const_mem_fun<ballot, uint64_t, &ballot::by_status>>,
                indexed_by<name("bysymbol"), const_mem_fun<ballot, uint64_t, &ballot::by_symbol>>,
                indexed_by<name("byendtime"), const_mem_fun<ballot, uint64_t, &ballot::by_end_time>>
            > decide_ballots_table;

            struct treasury {
                asset supply; //current supply
                asset max_supply; //maximum supply
                name access; //public, private, invite, membership
                name manager; //treasury manager

                string title;
                string description;
                string icon;

                uint32_t voters;
                uint32_t delegates;
                uint32_t committees;
                uint32_t open_ballots;

                bool locked; //locks all settings
                name unlock_acct; //account name to unlock
                name unlock_auth; //authorization name to unlock
                map<name, bool> settings; //setting_name -> on/off

                uint64_t primary_key() const { return supply.symbol.code().raw(); }
                EOSLIB_SERIALIZE(treasury, 
                    (supply)(max_supply)(access)(manager)
                    (title)(description)(icon)
                    (voters)(delegates)(committees)(open_ballots)
                    (locked)(unlock_acct)(unlock_auth)(settings))
            };
            typedef multi_index<name("treasuries"), treasury> treasuries_table;


            decide_config aux_get_telos_decide_config() {
                PRINT("vapaee::dex::dao::aux_get_telos_decide_config()\n");

                config_table table(vapaee::dex::dao::decide, vapaee::dex::dao::decide.value);
                auto ptr = table.begin();
                check(ptr != table.end(), create_error_name1(ERROR_AGTDC_1, vapaee::dex::dao::decide).c_str());
                return *ptr;
            }

            asset aux_get_telos_decide_ballot_fee() {
                decide_config configs = aux_get_telos_decide_config();
                return configs.fees[name("ballot")];
            }

            name aux_get_available_ballot_name() {
                decide_ballots_table ballots(vapaee::dex::dao::decide, vapaee::dex::dao::decide.value);
                return name(ballots.available_primary_key());
            }

            asset aux_get_telos_decide_treasury_supply(const symbol_code &sym_code) {
                treasuries_table table(vapaee::dex::dao::decide, vapaee::dex::dao::decide.value);
                auto treasury_ptr = table.find(sym_code.raw());
                check(treasury_ptr != table.end(), create_error_symcode1(ERROR_AGTDTS_1, sym_code).c_str());
                return treasury_ptr->supply;
            }

            // ---------------------------------------------

            name aux_check_name_from_string(string str) {
                return vapaee::utils::check_name_from_string(str);
            }

            symbol_code aux_check_symbol_code_from_string(string str) {
                return vapaee::utils::check_symbol_code_from_string(str);
            }

            float aux_check_float_from_string(string str) {
                return vapaee::utils::check_float_from_string(str);
            }

            uint32_t aux_check_integer_from_string(string str) {
                return vapaee::utils::check_integer_from_string(str);
            }

            asset aux_check_asset_from_string(string str) {
                return vapaee::utils::check_asset_from_string(str);
            }

            state get() {
                return vapaee::dex::global::get();
            }

            void set( const state& conf) {
                vapaee::dex::global::set(conf);
            }        

            void state_set_maker_fee(asset fee) {
                state entry_stored = get();
                entry_stored.maker_fee = fee;
                set(entry_stored);
            }

            void state_set_taker_fee(asset fee) {
                state entry_stored = get();
                entry_stored.taker_fee = fee;
                set(entry_stored);
            }

            void state_set_swap_fee(asset fee) {
                state entry_stored = get();
                entry_stored.swap_fee = fee;
                set(entry_stored);
            }

            void state_set_pprune( uint32_t enties) {
                state entry_stored = get();
                entry_stored.pprune = enties;
                set(entry_stored);
            }

            void state_set_bprune( uint32_t enties) {
                state entry_stored = get();
                entry_stored.bprune = enties;
                set(entry_stored);
            }

            void state_set_regcost(const asset& cost) {
                state entry_stored = get();
                entry_stored.regcost = cost;
                set(entry_stored);
            }

            void state_set_approvalmin(float min) {
                state entry_stored = get();
                entry_stored.approvalmin = min;
                set(entry_stored);
            }

            void state_set_reward(string name, string type, float value) {
                state entry_store = get();
                if (type == "exp") {
                    if (name == "trademaker")
                        entry_store.maker_exp_reward = value;
                    else if (name == "tradetaker")
                        entry_store.taker_exp_reward = value;
                    else if (name == "delmarkets")
                        entry_store.maint_reward_delmarkets_exp = value;
                    else if (name == "points")
                        entry_store.maint_reward_points_exp = value;
                    else if (name == "ballots")
                        entry_store.maint_reward_ballots_exp = value;
                } else if (type == "pts") {
                    if (name == "trademaker")
                        entry_store.maker_pts_reward = value;
                    else if (name == "tradetaker")
                        entry_store.taker_pts_reward = value;
                    else if (name == "delmarkets")
                        entry_store.maint_reward_delmarkets_pts = value;
                    else if (name == "points")
                        entry_store.maint_reward_points_pts = value;
                    else if (name == "ballots")
                        entry_store.maint_reward_ballots_pts = value;
                }
                set(entry_store);
            }

            bool aux_is_token_whitelisted(const symbol_code &sym_code) {
                whitelist list(contract, contract.value);
                auto itr = list.find(sym_code.raw());
                if (itr != list.end()) {
                    return true;
                }
                return false;
            }

            void aux_process_ballot_to_ban_token(symbol_code & symcode, name tcontract, name ballot_name) {
                PRINT("vapaee::dex::dao::aux_process_ballot_to_ban_token()\n");
                PRINT(" symcode: ", symcode.to_string(), "\n");
                PRINT(" contract: ", contract.to_string(), "\n");
                PRINT(" ballot_name: ", ballot_name.to_string(), "\n");
                
                whitelist white_table(contract, contract.value);
                auto wptr = white_table.find(symcode.raw());
                check(wptr == white_table.end(), create_error_symcode1(ERROR_APBTBT_1, symcode).c_str());

                // auto wptr = white_table.find(symcode.raw());
                // check(wptr == white_table.end(), create_error_symcode1(ERROR_APBTBT_1, symcode).c_str());
                stats stats_table(tcontract, symcode.raw());
                auto stptr = stats_table.find(symcode.raw());
                check(stptr != stats_table.end(), create_error_name1(ERROR_APBTBT_2, contract).c_str());
                uint8_t precision = stptr->supply.symbol.precision();
                PRINT(" -> stptr->supply: ", stptr->supply.to_string(), "\n");
                PRINT(" -> precision: ", std::to_string((int) precision), "\n");

                blacklist black_table(contract, contract.value);
                auto bptr = black_table.find(symcode.raw());
                check(bptr == black_table.end(), create_error_symcode1(ERROR_APBTBT_3, symcode).c_str());

                black_table.emplace(contract, [&](auto &a){
                    a.id = black_table.available_primary_key();
                    a.symbol = symcode;
                    a.contract = tcontract;
                    a.ballot = ballot_name;
                    a.precision = precision;
                });
                
                PRINT("vapaee::dex::dao::aux_process_ballot_to_ban_token() ...\n");
            }

            void aux_check_ballot_can_be_created(name operation, vector<string> params) {
                PRINT("vapaee::dex::dao::aux_check_ballot_can_be_created()\n");

                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                PRINT(" _now:    ", std::to_string((unsigned long)_now.utc_seconds), "\n");

                ballots list(contract, contract.value);
                auto index = list.get_index<name("finished")>();
                auto itr = index.lower_bound(_now.utc_seconds);


                for (; itr != index.end(); itr++) {
                    PRINT(" ballot: (", std::to_string((unsigned long)itr->finished.utc_seconds), ") ", itr->ballot_name.to_string(), " - ", itr->operation.to_string(), "\n");

                    // discard different operations
                    if (itr->operation != operation) continue; 

                    // discard finished ballots
                    if (itr->finished <= _now) continue;

                    bool are_the_same = true;
                    for (int i=0; i<itr->params.size(); i++) {
                        PRINT("   param[",i,"]: ", itr->params[i].c_str(), "\n");
                        if (itr->params[i] != params[i]) {
                            are_the_same = false;
                            break; // all good, at least one param is different
                        }
                    }

                    // fails if the ballots is the same as passed by params
                    check(!are_the_same, create_error_name1(ERROR_ACBABC_3, operation).c_str());
                }

                // if we reach this point, there's no other ballot opened with same data at the moment
                PRINT("vapaee::dex::dao::aux_check_ballot_can_be_created() ...\n");
            }

            void action_start_ballot_on (name operation, vector<string> params, string arguments, name feepayer) {
                
                PRINT("vapaee::dex::dao::action_start_ballot_on()\n");
                PRINT(" operation: ", operation.to_string(), "\n");
                for (int i=0; i<params.size(); i++) {
                    PRINT(" params[",i,"]: ", params[i].c_str(), "\n");
                }
                PRINT(" feepayer: ", feepayer.to_string(), "\n");

                require_auth(feepayer);
                PRINT(" require_auth(feepayer); OK\n");

                check(arguments.size() < 1024, create_error_id1(ERROR_ASBO_1, arguments.size()).c_str());

                // validating operation
                if (
                    operation != name("bantoken")     && 
                    operation != name("savetoken")    && 
                    operation != name("makerfee")     && 
                    operation != name("takerfee")     && 
                    operation != name("pointsprune")  &&
                    operation != name("ballotsprune") &&
                    operation != name("approvalmin")  &&
                    operation != name("regcost") &&
                    operation != name("setreward")
                ) {
                    check(false, create_error_name1(ERROR_ASBO_2, operation).c_str());
                }
                PRINT(" operation OK\n");

                // validating params
                if (operation == name("bantoken")  ||
                    operation == name("savetoken")
                ) {
                    PRINT(" checking 2 params...\n");
                    string param1 = params[0];
                    string param2 = params[1];
                    symbol_code sym_code = aux_check_symbol_code_from_string(param1);
                    name contract = aux_check_name_from_string(param2);
                }

                if (operation == name("approvalmin")  
                ) {
                    PRINT(" checking 1 param...\n");
                    string param1 = params[0];
                    float value = aux_check_float_from_string(param1);
                    check((0 < value) && (value <= 1), "percentage format error");
                    PRINT(" value OK!\n");
                }

                if (operation == name("setreward")) {
                    PRINT(" checking 3 params...\n");
                    string reward_name = params[0];
                    check(
                        (reward_name == "trademaker") ||
                        (reward_name == "tradetaker") ||
                        (reward_name == "delmarkets") ||
                        (reward_name == "points")     ||
                        (reward_name == "ballots"),
                        "invalid reward name"
                    );
                    string reward_type = params[1];
                    check(
                        (reward_type == "exp") || (reward_type == "pts"),
                        "invalid reward type"
                    );
                    float reward_value = aux_check_float_from_string(params[2]);
                }

                if (operation == name("pointsprune")  ||
                    operation == name("ballotsprune")
                ) {
                    PRINT(" checking 1 param...\n");
                    string param1 = params[0];
                    uint32_t value = aux_check_integer_from_string(param1);
                    PRINT(" value OK!\n");
                }

                if (operation == name("regcost")  ||
                    operation == name("makerfee") || 
                    operation == name("takerfee")
                ) {
                    PRINT(" checking 1 param...\n");
                    string param1 = params[0];
                    asset value = aux_check_asset_from_string(param1);
                    PRINT(" value OK!\n");
                }

                PRINT(" params OK!\n");

                // check if ballot can be created or there's already an open ballot for the same operation
                aux_check_ballot_can_be_created(operation, params);

                // find out what is the ballot fee
                asset ballot_fee = aux_get_telos_decide_ballot_fee();
                PRINT(" -> ballot_fee: ", ballot_fee.to_string(), "\n");
                check(ballot_fee.symbol.code() == vapaee::utils::SYS_TKN_CODE,
                    create_error_symcode2(
                        ERROR_ASBO_3,
                        ballot_fee.symbol.code(),
                        vapaee::utils::SYS_TKN_CODE).c_str());

                // convert to internal 8 decimal representation
                asset extended = aux_extend_asset(ballot_fee);
                PRINT(" -> ballot_fee extended: ", extended.to_string(), "\n");

                // charge feepayer for the fees to pay telos.decide ballot service
                vapaee::dex::fees::aux_delete_fees(vapaee::dex::fees::concept_ballot, feepayer);

                // deposit ballot fee in telos decide contract
                PRINT(" -> transfer() ", ballot_fee.to_string(), " to ", vapaee::dex::dao::decide, "\n");
                action(
                    permission_level{contract,name("active")},
                    vapaee::utils::SYS_TKN_CONTRACT,
                    name("transfer"),
                    std::make_tuple(contract, vapaee::dex::dao::decide, ballot_fee, string("deposit"))
                ).send();

                // + at this point it should be a treasury created for VOTE token in telos decide contract
                // + at this point it this contract name should be registered as voter in that treasury

                // create ballot on Telos Decide contract
                name ballot_name = aux_get_available_ballot_name();
                PRINT(" -> ballot_name ", ballot_name.to_string(), "\n");
                name category = name("poll");
                symbol treasury_symbol = vapaee::dex::dao::treasury_symbol;
                name voting_method = name("1token1vote");
                vector<name> initial_options = {name("yes"), name("no"), name("abstain")};


                PRINT(" -> newballot() ",
                    ballot_name.to_string(),",",
                    category.to_string(),",",
                    treasury_symbol.code().to_string(),",",
                    voting_method.to_string(),  "\n");

                action(
                    permission_level{contract,name("active")},
                    vapaee::dex::dao::decide,
                    name("newballot"),
                    std::make_tuple(
                        ballot_name,
                        category,
                        contract,
                        treasury_symbol,
                        voting_method,
                        initial_options
                    )
                ).send();

                // open ballot for votting in Telos Decide contract
                uint32_t _15_days_in_sec = 15 * 24 * 60 * 60;
                //XXX: MODIFIED FOR TESTING
                // _15_days_in_sec = 2;
                time_point_sec end_time = time_point_sec(current_time_point().sec_since_epoch() + _15_days_in_sec);

                PRINT(" -> now ", std::to_string((unsigned long) vapaee::dex::global::get_now_time_point_sec().sec_since_epoch()), "\n");
                PRINT(" -> openvoting() ", ballot_name.to_string(),",", std::to_string((unsigned long) end_time.sec_since_epoch()), "\n");

                action(
                    permission_level{contract,name("active")},
                    vapaee::dex::dao::decide,
                    name("openvoting"),
                    std::make_tuple(
                        ballot_name,
                        end_time
                    )
                ).send();

                ballots ball_table(contract, contract.value);
                auto index = ball_table.get_index<name("ballotname")>();
                auto ptr = index.find(ballot_name.value);
                check(ptr == index.end(), create_error_name1(ERROR_ASBO_4, ballot_name).c_str());        

                ball_table.emplace(contract, [&]( auto &a){
                    a.id = ball_table.available_primary_key();
                    a.ballot_name = ballot_name;
                    a.operation = operation;
                    a.params = params;
                    a.arguments = arguments;
                    a.feepayer = feepayer;
                    a.started = vapaee::dex::global::get_now_time_point_sec();
                    a.finished = time_point_sec::maximum();
                });

                if ( operation == name("bantoken") ) {
                    string param1 = params[0];
                    string param2 = params[1];
                    symbol_code sym_code = aux_check_symbol_code_from_string(param1);
                    name contract = aux_check_name_from_string(param2);
                    aux_process_ballot_to_ban_token(sym_code, contract, ballot_name);
                }

                PRINT("vapaee::dex::dao::action_start_ballot_on() ...\n");
                
            }

            void aux_delete_token_markets(const symbol_code & sym_code) {
                PRINT("vapaee::dex::dao::aux_delete_token_markets()\n");
                PRINT(" sym_code: ",sym_code.to_string(),"\n");

                markets table(contract, contract.value);
                delmarkets deltable(contract, contract.value);
                ordersummary summarytable(contract, contract.value);

                // deleting all markets with this token as a currency
                auto currency_index = table.get_index<name("currency")>();
                PRINT(" for(currency_index)... \n");
                for (
                    auto ptr = currency_index.lower_bound(sym_code.raw());
                    ptr != currency_index.end();
                    ptr = currency_index.lower_bound(sym_code.raw())
                ) {
                    PRINT("  for (currency_index) ", ptr->to_string(), ", ", ptr->currency.to_string(), " \n");
                    if (sym_code == ptr->currency) {

                        // add this market to be slowly deleted (history and orders)
                        PRINT("  -> delmarkets.emplace(", ptr->to_string(), ")\n");
                        deltable.emplace(contract, [&](auto &a){
                            a.id = ptr->id;
                        });

                        // we delete de actual market
                        PRINT("  -> markets.erase(", ptr->to_string(), ")\n");
                        table.erase(*ptr);

                        // delete ordersummary unique entrance for this market
                        auto sptr = summarytable.find(ptr->id);
                        if (sptr != summarytable.end()) {
                            PRINT("  -> ordersummary.erase()\n");
                            summarytable.erase(*sptr);
                        }
                    } else {
                        PRINT(" break\n");
                        break; // avoid iterating to the end
                    }
                }

                // deleting all markets with this token as a commodity
                auto commodity_index = table.get_index<name("commodity")>();
                
                PRINT(" for(commodity_index)... \n");
                for (
                    auto ptr = commodity_index.lower_bound(sym_code.raw());
                    ptr != commodity_index.end();
                    ptr = commodity_index.lower_bound(sym_code.raw())
                ) {
                    PRINT("  for (commodity_index) ", ptr->to_string(), ", ", ptr->commodity.to_string(), " \n");
                    if (sym_code == ptr->commodity) {

                        // add this market to be slowly deleted (history and orders)
                        PRINT("  -> delmarkets.emplace(", ptr->to_string(), ")\n");
                        deltable.emplace(contract, [&](auto &a){
                            a.id = ptr->id;
                        });

                        // we delete de actual market
                        PRINT("  -> markets.erase(", ptr->to_string(),")\n");
                        table.erase(*ptr);

                        // delete ordersummary unique entrance for this market
                        auto sptr = summarytable.find(ptr->id);
                        if (sptr != summarytable.end()) {
                            PRINT("  -> ordersummary.erase()\n");
                            summarytable.erase(*sptr);
                        }
                    } else {
                        PRINT(" break\n");
                        break; // avoid iterating to the end
                    }
                }

                PRINT("vapaee::dex::dao::aux_delete_token_markets() ...\n");
            }

            void handler_ballot_result_for_savetoken(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_savetoken()\n");
                PRINT(" ballot.ballot_name: ",ballot.ballot_name.to_string(),"\n");
                PRINT(" ballot.params[0]: ",ballot.params[0],"\n");
                PRINT(" ballot.params[1]: ",ballot.params[1],"\n");
                PRINT(" approved: ",std::to_string(approved),"\n");
                PRINT(" total_voters: ",std::to_string(total_voters),"\n");

                string param1 = ballot.params[0];
                string param2 = ballot.params[1];
                symbol_code sym_code = aux_check_symbol_code_from_string(param1);
                name target_contract = aux_check_name_from_string(param2);          

                whitelist list(contract, contract.value);
                auto itr = list.find(sym_code.raw());

                bool found = (itr != list.end());
                PRINT(" -> found: ",std::to_string(found),"\n");

                if (approved) {
                    if (!found) {
                        bool blacklisted = vapaee::dex::security::aux_is_token_blacklisted(sym_code, target_contract);
                        check(!blacklisted, create_error_symcode2(ERROR_HBRFS_1, itr->symbol, sym_code).c_str());

                        list.emplace(contract, [&](auto &a){
                            a.symbol = sym_code;
                            a.contract = target_contract;
                            a.ballot = ballot.ballot_name;                            
                        });
                    }
                } else {
                    if (found) {
                        check(itr->symbol == sym_code, create_error_symcode2(ERROR_HBRFS_2, itr->symbol, sym_code).c_str());
                        list.erase(*itr);
                    }
                }
                PRINT("vapaee::dex::dao::handler_ballot_result_for_savetoken() ...\n");
            }

            void handler_ballot_result_for_bantoken(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_bantoken()\n");
                
                string param1 = ballot.params[0];
                symbol_code sym_code = aux_check_symbol_code_from_string(param1);

                if (approved) {
                    // double check if the token is not in the white list
                    check(!aux_is_token_whitelisted(sym_code), create_error_symcode1(ERROR_HBRFB_1, sym_code).c_str());

                    // approved, so the token must be removed from the token list.
                    tokens list(contract, contract.value);
                    auto ptr = list.find(sym_code.raw());
                    check(ptr != list.end(), create_error_symcode1(ERROR_HBRFB_2, sym_code).c_str());

                    if (ptr != list.end()) {
                        list.erase(*ptr);
                    }

                    // delete all markets were this tokens participates
                    aux_delete_token_markets(sym_code);

                } else {
                    // not approved, so the token must be removed from the blacklist.
                    blacklist blist(contract, contract.value);
                    auto index = blist.get_index<name("symbol")>();
                    auto itr = index.lower_bound(sym_code.raw());

                    if (itr != index.end()) {
                        // token is free to operate again
                        blist.erase(*itr);
                    }
                }
                
                PRINT("vapaee::dex::dao::handler_ballot_result_for_bantoken() ...\n");
            }

            void handler_ballot_result_for_makerfee(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_makerfee()\n");

                string param1 = ballot.params[0];
                asset value = aux_check_asset_from_string(param1);

                PRINT(" -> value: ",value.to_string(),"\n");

                if (approved) {
                    PRINT(" -> get().maker_fee: ",get().maker_fee,"\n");
                    state_set_maker_fee(value);
                    PRINT(" -> get().maker_fee: ",get().maker_fee,"\n");
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_makerfee() ...\n");
            }

            void handler_ballot_result_for_takerfee(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_takerfee()\n");

                string param1 = ballot.params[0];
                asset value = aux_check_asset_from_string(param1);

                PRINT(" -> value: ",value.to_string(),"\n");
                
                if (approved) {
                    PRINT(" -> get().taker_fee: ",get().taker_fee,"\n");
                    state_set_taker_fee(value);
                    PRINT(" -> get().taker_fee: ",get().taker_fee,"\n");
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_takerfee() ...\n");
            }

            void handler_ballot_result_for_swapfee(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_swapfee()\n");

                string param1 = ballot.params[0];
                asset value = aux_check_asset_from_string(param1);

                PRINT(" -> value: ",value.to_string(),"\n");
                
                if (approved) {
                    PRINT(" -> get().swap_fee: ",get().swap_fee,"\n");
                    state_set_swap_fee(value);
                    PRINT(" -> get().swap_fee: ",get().swap_fee,"\n");
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_swapfee() ...\n");
            }

            void handler_ballot_result_for_ballotsprune(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_ballotsprune()\n");

                string param1 = ballot.params[0];
                uint32_t value = aux_check_integer_from_string(param1);

                if (approved) {
                    state_set_bprune(value);
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_ballotsprune() ...\n");
            }

            void handler_ballot_result_for_pointsprune(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_pointsprune()\n");

                string param1 = ballot.params[0];
                uint32_t value = aux_check_integer_from_string(param1);

                if (approved) {
                    state_set_pprune(value);
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_pointsprune() ...\n");
            }

            void handler_ballot_result_for_regcost(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_regcost()\n");

                string param1 = ballot.params[0];
                asset value = aux_check_asset_from_string(param1);

                if (approved) {
                    state_set_regcost(value);
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_regcost() ...\n");
            }

            void handler_ballot_result_for_approvalmin(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_approvalmin()\n");

                string param1 = ballot.params[0];
                float value = aux_check_float_from_string(param1);

                if (approved) {
                    state_set_approvalmin(value);
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_approvalmin() ...\n");
            }

            void handler_ballot_result_for_setreward(const ballots_table & ballot, bool approved, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result_for_setreward()\n");

                string reward_name = ballot.params[0];
                string reward_type = ballot.params[1];
                float reward_value = aux_check_float_from_string(ballot.params[2]);

                if (approved) {
                    state_set_reward(reward_name, reward_type, reward_value);
                }

                PRINT("vapaee::dex::dao::handler_ballot_result_for_setreward() ...\n");
            }

            void handler_ballot_result(name ballot_name, map<name, asset> final_results, uint32_t total_voters) {
                PRINT("vapaee::dex::dao::handler_ballot_result()\n");
                PRINT(" ballot_name: ", ballot_name.to_string(), "\n");
                
                for (auto it = final_results.begin(); it != final_results.end(); ++it) {
                    PRINT(" final_results[",it->first.to_string(),"]: ", it->second.to_string(), "\n");
                }
                PRINT(" total_voters: ", std::to_string((unsigned long)total_voters), "\n");

                // was it approved???? ----------------
                bool approved = final_results[name("yes")].amount > final_results[name("no")].amount;
                bool accepted = true;
                asset curr_supply = aux_get_telos_decide_treasury_supply(vapaee::dex::dao::treasury_symbol.code());
                float percent = vapaee::dex::global::get().approvalmin;
                asset participation = final_results[name("yes")] + final_results[name("no")] + final_results[name("abstain")];
                if (participation.amount < percent * curr_supply.amount) {
                    approved = false;
                    accepted = false;
                    PRINT(" NOT APPROVED because low participation ratio\n");
                    PRINT(" approvalmin: ", std::to_string(percent), "\n");
                    PRINT(" participation: ", participation.to_string(), "\n");
                    PRINT(" curr_supply: ", curr_supply.to_string(), "\n");
                }
                PRINT(" approved: ", approved ? "YES": "NO", "\n");
                PRINT(" accepted: ", accepted ? "YES": "NO", "\n");

                // search locally for the ballot data
                ballots ball_table(contract, contract.value);
                // auto ballot = ball_table.get(ballot_name.value);
                auto ptr = ball_table.find(ballot_name.value);
                check(ptr != ball_table.end(), create_error_name1(ERROR_HBR_1, ballot_name).c_str());

                PRINT(" ptr: ", ptr->ballot_name.to_string(), " ", ptr->operation.to_string(), "\n");
                
                ball_table.modify(*ptr, same_payer, [&](auto &a){
                    a.finished = vapaee::dex::global::get_now_time_point_sec();
                    a.approved = approved;
                    a.accepted = accepted;
                    a.results = final_results;
                });

                if (accepted) {
                    auto ballot = *ptr;
                    switch(ballot.operation.value) {
                        case name("savetoken").value:
                            handler_ballot_result_for_savetoken(ballot, approved, total_voters);
                            break;
                        case name("bantoken").value:
                            handler_ballot_result_for_bantoken(ballot, approved, total_voters);
                            break;
                        case name("makerfee").value:
                            handler_ballot_result_for_makerfee(ballot, approved, total_voters);
                            break;
                        case name("takerfee").value:
                            handler_ballot_result_for_takerfee(ballot, approved, total_voters);
                            break;
                        case name("pointsprune").value:
                            handler_ballot_result_for_pointsprune(ballot, approved, total_voters);
                            break;
                        case name("ballotsprune").value:
                            handler_ballot_result_for_ballotsprune(ballot, approved, total_voters);
                            break;
                        case name("approvalmin").value:
                            handler_ballot_result_for_approvalmin(ballot, approved, total_voters);
                            break;
                        case name("regcost").value:
                            handler_ballot_result_for_regcost(ballot, approved, total_voters);
                            break;
                        case name("setreward").value:
                            handler_ballot_result_for_setreward(ballot, approved, total_voters);
                            break;
                        default:
                            check(false, create_error_name1(ERROR_HBR_2, ballot.operation).c_str()); 
                            break;
                    }
                }

                PRINT("vapaee::dex::dao::handler_ballot_result() ...\n");
                
            }
            
        };     
    };
};
