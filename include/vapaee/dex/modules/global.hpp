#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/error.hpp>

namespace vapaee {
    namespace dex {

        using namespace error;        

        namespace global {

            const symbol system_symbol = symbol(symbol_code("TLOS"), 4);
            const symbol fee_symbol = symbol(symbol_code("FEE"), 8);


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
                entry_stored.hprune = conf.hprune;
                entry_stored.kprune = conf.kprune;
                entry_stored.bprune = conf.bprune;
                entry_stored.eprune = conf.eprune;
                entry_stored.pprune = conf.pprune;
                entry_stored.approvalmin = conf.approvalmin;
                entry_stored.regcost = conf.regcost;
                entry_stored.next_market = conf.next_market;
                AUX_DEBUG_CODE(entry_stored.time_offset = conf.time_offset;)
                get_singleton().set(entry_stored, contract);
            }


            time_point_sec get_now_time_point_sec() {
                PRINT("vapaee::dex::global::get_now_time_point_sec()\n");

                uint32_t offset = 0;
                PRINT(" -> offset: ", std::to_string((unsigned long)offset), "\n");
                AUX_DEBUG_CODE(
                    offset = get().time_offset;
                    PRINT(" -> offset: ", std::to_string((unsigned long)offset), " (updated)\n");
                )
                time_point_sec _now = time_point_sec(current_time_point().sec_since_epoch() - offset);
                
                PRINT(" -> _now: ", std::to_string((unsigned long)_now.utc_seconds), " (updated)\n");
                PRINT("vapaee::dex::global::get_now_time_point_sec() ...\n");
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
                new_state.taker_fee = asset(2500000, fee_symbol); // 2.5% fees for market takers
                new_state.maker_fee = asset(1000000, fee_symbol); // 1.0% fees for market makers
                new_state.hprune = 365;       // 365 days old history entry should be considered expired and must be deleted
                new_state.kprune = 365;       // 365 days old history block entry should be considered expired and must be deleted
                new_state.bprune = 1000;      // no more than 1000 entries allowed in the ballots table.
                new_state.eprune = 60;        // 60 days old event should be considered expired and must be deleted
                new_state.pprune = 6;         // 6 weeks old points should be considered expired and must be deleted
                new_state.approvalmin = 0.25; // 25% of participation must be reached in order to approve a ballot
                new_state.regcost = asset(1000000, system_symbol);
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
