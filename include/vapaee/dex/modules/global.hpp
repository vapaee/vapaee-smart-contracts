#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>

namespace vapaee {
    namespace dex { 

        namespace global {

            inline name get_self() {
                return vapaee::dex::contract;
            }

            const symbol system_symbol = symbol(vapaee::utils::SYS_TKN_CODE, 4);
            const symbol fee_symbol = symbol(vapaee::utils::FEE_TKN_CODE, 8);
                        
            inline global_state_singleton get_singleton() {
                return global_state_singleton(get_self(), get_self().value);
            };

            state get() {
                check(get_singleton().exists(), create_error_name1(ERROR_GC_1, name("state")).c_str());
                return get_singleton().get();
            }

            void set( const state& conf) {
                state entry_stored = get();
                entry_stored.maker_fee = conf.maker_fee;
                entry_stored.taker_fee = conf.taker_fee;
                entry_stored.swap_fee = conf.swap_fee;
                entry_stored.bprune = conf.bprune;
                entry_stored.pprune = conf.pprune;
                entry_stored.approvalmin = conf.approvalmin;
                entry_stored.regcost = conf.regcost;
                entry_stored.maker_pts_reward = conf.maker_pts_reward;
                entry_stored.maker_exp_reward = conf.maker_exp_reward;
                entry_stored.taker_pts_reward = conf.taker_pts_reward;
                entry_stored.taker_exp_reward = conf.taker_exp_reward;

                entry_stored.maint_reward_delmarkets_exp = conf.maint_reward_delmarkets_exp;
                entry_stored.maint_reward_points_exp = conf.maint_reward_points_exp;
                entry_stored.maint_reward_ballots_exp = conf.maint_reward_ballots_exp;

                entry_stored.maint_reward_delmarkets_pts = conf.maint_reward_delmarkets_pts;
                entry_stored.maint_reward_points_pts = conf.maint_reward_points_pts;
                entry_stored.maint_reward_ballots_pts = conf.maint_reward_ballots_pts;

                entry_stored.next_market = conf.next_market;
                entry_stored.paused = conf.paused;
                SACAME_EL_NOW(entry_stored.now = conf.now;)
                get_singleton().set(entry_stored, get_self());
            }

            time_point_sec get_N_seconds_from_point_sec(time_point_sec date, uint64_t  seconds) {            
                uint32_t offset = 0;
                
                time_point_sec _then = time_point_sec(date.sec_since_epoch() + seconds - offset);
                return _then;
            }
        
            time_point_sec get_N_days_from_point_sec(time_point_sec date, int days) {            
                uint32_t offset = 0;
                uint32_t _N_days_in_sec = days * 24 * 60 * 60;
                time_point_sec _now = time_point_sec(date.sec_since_epoch() + _N_days_in_sec - offset);
                
                return _now;
            }
        
            time_point_sec get_now_time_point_sec() {
                return time_point_sec(current_time_point().sec_since_epoch());
            }

            uint32_t get_current_week_number() {
                PRINT("vapaee::dex::global::get_current_week_number()\n");
                time_point_sec _now = get_now_time_point_sec();
                float one_week = 7 * 24 * 60 * 60;
                float epoch_secs = (float) _now.sec_since_epoch();
                uint32_t week = uint32_t (  epoch_secs / one_week );
                PRINT("vapaee::dex::global::get_current_week_number() ... -> ",std::to_string((unsigned long)week),"\n");
                return week;
            }



            void init() {
                PRINT("vapaee::dex::global::init()\n");
                //authenticate
                require_auth(get_self());

                //open state singleton
                global_state_singleton states(get_self(), get_self().value);

                if (states.exists()) {
                    // we avoid re initialization but don't throw an error
                    PRINT("vapaee::dex::global::init() ...\n");
                    return;
                }

                state new_state;
                new_state.taker_fee = asset(500000, fee_symbol); // 0.5% fees for market takers
                new_state.maker_fee = asset(100000, fee_symbol); // 0.1% fees for market makers
                new_state.swap_fee  = asset(500000, fee_symbol); // 0.5% fees for any swap
                new_state.bprune = 1000;      // no more than 1000 entries allowed in the ballots table.
                new_state.pprune = 6;         // 6 weeks old points should be considered expired and must be deleted
                new_state.approvalmin = 0.25; // 25% of participation must be reached in order to approve a ballot
                new_state.regcost = asset(1000000, system_symbol);
                new_state.maker_pts_reward = 1;
                new_state.maker_exp_reward = 0.5;
                new_state.taker_pts_reward = 2;
                new_state.taker_exp_reward = 1;

                new_state.maint_reward_delmarkets_exp = 1;
                new_state.maint_reward_points_exp = 0.5;
                new_state.maint_reward_ballots_exp = 1;
                new_state.maint_reward_update_loans_exp = 0.2;

                new_state.maint_reward_delmarkets_pts = 1;
                new_state.maint_reward_points_pts = 2;
                new_state.maint_reward_ballots_pts = 1;
                new_state.maint_reward_update_loans_pts = 0.1;

                new_state.next_market = 0;
                new_state.paused = false;
                SACAME_EL_NOW(new_state.now = get_now_time_point_sec().sec_since_epoch();)

                states.set(new_state, get_self());
                PRINT("vapaee::dex::global::init() ...\n");

            }

            AUX_DEBUG_CODE (
                // This method is only for testing purposes
                void action_updatenow() {
                    PRINT("vapaee::dex::global::action_updatenow()\n");
                    //authenticate
                    require_auth(get_self());

                    //open state singleton
                    global_state_singleton states(get_self(), get_self().value);

                    //validate
                    check(states.exists(), "ERROR-AUN-1");

                    auto state = states.get();
                    SACAME_EL_NOW(state.now = get_now_time_point_sec().sec_since_epoch();)
                    states.set(state, get_self());
                    PRINT("vapaee::dex::global::action_updatenow() ...\n");
                }
            )
            
            void action_pause(bool paused) {
                PRINT("vapaee::dex::global::action_pause()\n");
                //authenticate
                require_auth(get_self());

                //open state singleton
                global_state_singleton states(get_self(), get_self().value);

                //validate
                check(states.exists(), "ERROR-AUN-1");

                auto state = states.get();
                state.paused = paused;
                states.set(state, get_self());
                PRINT("vapaee::dex::global::action_pause() ...\n");
            }

            AUX_DEBUG_CODE(
                void action_set_time_offset(uint32_t offset) {
                    PRINT("vapaee::dex::global::action_set_time_offset()\n");
                    PRINT(" offset: ", offset, "\n");
                    auto state = get();
                    // state.time_offset = offset;
                    set(state);
                    PRINT("vapaee::dex::global::action_set_time_offset() ...\n");
                }
            )            
        };     
    };
};
