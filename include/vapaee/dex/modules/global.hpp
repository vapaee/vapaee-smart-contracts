#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
// #include <vapaee/dex/modules/error.hpp>

namespace vapaee {
    namespace dex { 

        namespace global {

            const symbol system_symbol = symbol(vapaee::utils::SYS_TKN_CODE, 4);
            const symbol fee_symbol = symbol(vapaee::utils::FEE_TKN_CODE, 8);
                        
            inline global_state_singleton get_singleton() {
                return global_state_singleton(contract, contract.value);
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
                entry_stored.hprune = conf.hprune;
                entry_stored.kprune = conf.kprune;
                entry_stored.bprune = conf.bprune;
                entry_stored.eprune = conf.eprune;
                entry_stored.pprune = conf.pprune;
                entry_stored.approvalmin = conf.approvalmin;
                entry_stored.regcost = conf.regcost;
                entry_stored.maker_pts_reward = conf.maker_pts_reward;
                entry_stored.maker_exp_reward = conf.maker_exp_reward;
                entry_stored.taker_pts_reward = conf.taker_pts_reward;
                entry_stored.taker_exp_reward = conf.taker_exp_reward;

                entry_stored.maint_reward_delmarkets_exp = conf.maint_reward_delmarkets_exp;
                entry_stored.maint_reward_history_exp = conf.maint_reward_history_exp;
                entry_stored.maint_reward_events_exp = conf.maint_reward_events_exp;
                entry_stored.maint_reward_points_exp = conf.maint_reward_points_exp;
                entry_stored.maint_reward_ballots_exp = conf.maint_reward_ballots_exp;

                entry_stored.maint_reward_delmarkets_pts = conf.maint_reward_delmarkets_pts;
                entry_stored.maint_reward_history_pts = conf.maint_reward_history_pts;
                entry_stored.maint_reward_events_pts = conf.maint_reward_events_pts;
                entry_stored.maint_reward_points_pts = conf.maint_reward_points_pts;
                entry_stored.maint_reward_ballots_pts = conf.maint_reward_ballots_pts;

                entry_stored.next_market = conf.next_market;
                AUX_DEBUG_CODE(entry_stored.time_offset = conf.time_offset;)
                get_singleton().set(entry_stored, contract);
            }
        
            time_point_sec get_N_days_from_point_sec(time_point_sec date, int days) {            
                uint32_t offset = 0;
                AUX_DEBUG_CODE(
                    offset = get().time_offset;
                )

                uint32_t _N_days_in_sec = days * 24 * 60 * 60;
                time_point_sec _now = time_point_sec(date.sec_since_epoch() + _N_days_in_sec - offset);
                
                return _now;
            }
        
            time_point_sec get_now_time_point_sec() {
                uint32_t offset = 0;
                AUX_DEBUG_CODE(
                    offset = get().time_offset;
                )
                time_point_sec _now = time_point_sec(current_time_point().sec_since_epoch() - offset);
                return _now;
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
                require_auth(contract);

                //open state singleton
                global_state_singleton states(contract, contract.value);

                //validate
                check(!states.exists(), ERROR_AIC_1);

                state new_state;
                new_state.taker_fee = asset(500000, fee_symbol); // 0.5% fees for market takers
                new_state.maker_fee = asset(100000, fee_symbol); // 0.1% fees for market makers
                new_state.swap_fee  = asset(500000, fee_symbol); // 0.5% fees for any swap
                new_state.hprune = 365;       // 365 days old history entry should be considered expired and must be deleted
                new_state.kprune = 365;       // 365 days old history block entry should be considered expired and must be deleted
                new_state.bprune = 1000;      // no more than 1000 entries allowed in the ballots table.
                new_state.eprune = 60;        // 60 days old event should be considered expired and must be deleted
                new_state.pprune = 6;         // 6 weeks old points should be considered expired and must be deleted
                new_state.approvalmin = .25f; // 25% of participation must be reached in order to approve a ballot
                new_state.regcost = asset(1000000, system_symbol);
                new_state.maker_pts_reward = 1;
                new_state.maker_exp_reward = .5f;
                new_state.taker_pts_reward = 2;
                new_state.taker_exp_reward = 1;

                new_state.maint_reward_delmarkets_exp = 1;
                new_state.maint_reward_history_exp = 1;
                new_state.maint_reward_events_exp = .5f;
                new_state.maint_reward_points_exp = .5f;
                new_state.maint_reward_ballots_exp = 1;

                new_state.maint_reward_delmarkets_pts = 1;
                new_state.maint_reward_history_pts = 1;
                new_state.maint_reward_events_pts = 2;
                new_state.maint_reward_points_pts = 2;
                new_state.maint_reward_ballots_pts = 1;

                new_state.next_market = 0;
                AUX_DEBUG_CODE(new_state.time_offset = 0;)

                states.set(new_state, contract);
                PRINT("vapaee::dex::global::init() ...\n");

            }
            

            AUX_DEBUG_CODE(
                void action_set_time_offset(uint32_t offset) {
                    PRINT("vapaee::dex::global::action_set_time_offset()\n");
                    PRINT(" offset: ", offset, "\n");
                    auto state = get();
                    state.time_offset = offset;
                    set(state);
                    PRINT("vapaee::dex::global::action_set_time_offset() ...\n");
                }
            )
            

            
        };     
    };
};
