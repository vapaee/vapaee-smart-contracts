/*#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/wrap/tables.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/wrap/modules/rex.hpp>


namespace vapaee {
    namespace wrap {
        namespace global {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            globalstate_table get_global_state() {
                PRINT("vapaee::wrap::global::get_global_state()\n");
                globalstate gs_table(get_self(), get_self().value);
                auto itr = gs_table.begin();
                check(itr != gs_table.end(), ERROR_GGS_1);
                return *itr;
            }

            void add_loan(tloswrap tokens) {
                PRINT("vapaee::wrap::global::add_loan()\n");

                globalstate gs_table(get_self(), get_self().value);
                auto itr = gs_table.begin();
                check(itr != gs_table.end(), ERROR_AL_1);
                
                gs_table.modify(itr, same_payer, [&](auto& state) {
                    state.loans++;
                    state.current_shot++;
                    state.participation.tlosv += tokens.tlosv;
                    state.participation.tlosw += tokens.tlosw;
                });
            }

            uint64_t get_current_shot() {
                PRINT("vapaee::wrap::global::get_current_shot()\n");
                return get_global_state().current_shot;
            }

            snapshot_table get_snapshot(uint64_t shot) {
                PRINT("vapaee::wrap::global::get_snapshot()\n");
                snapshots table(get_self(), get_self().value);
                auto itr = table.find(shot);
                check(itr != table.end(), ERROR_GS_1);
                return *itr;
            }

            void create_snapshot() {
                PRINT("vapaee::wrap::global::create_snapshot()\n");

                // table variable
                snapshots table(get_self(), get_self().value);

                // init ids and gains
                uint64_t next_id = table.available_primary_key();
                asset gains = asset(0, vapaee::utils::SYS_TKN_SYMBOL);
                asset current_assets = vapaee::wrap::rex::get_total_assets_in_tlos();

                // calculate gains between the las snapshot and this one
                if (next_id > 0) {
                    uint64_t last_id = next_id - 1;
                    auto last_shot = table.find(last_id);
                    gains = current_assets - last_shot->after.total_assets;
                }

                // creaate new snapshot and wrote down the gains only if there is a gain
                if (gains.amount > 0) {
                    table.emplace(get_self(), [&](auto& snapshot) {
                        snapshot.id = next_id;
                        snapshot.gains = gains;
                        snapshot.before.loans = get_global_state().loans;
                        snapshot.before.total_assets = current_assets;
                        //snapshot.before.participation = get_global_state().participation;
                        snapshot.participation = get_global_state().participation;
                        snapshot.assigned = asset(0, vapaee::utils::SYS_TKN_SYMBOL);
                        snapshot.rex_maturities.clear();
                        snapshot.remaining = get_global_state().loans;
                    });
                }
            }

            void update_snapshot() {
                PRINT("vapaee::wrap::global::update_snapshot()\n");

                snapshots table(get_self(), get_self().value);
                auto itr = table.find(get_global_state().current_shot);
                check(itr != table.end(), ERROR_US_1);

                table.modify(itr, same_payer, [&](auto& snapshot) {
                    snapshot.after.loans = get_global_state().loans;
                    snapshot.after.total_assets = vapaee::wrap::rex::get_total_assets_in_tlos();
                    // snapshot.after.participation = get_global_state().participation;

                    // must be a registered change between before and after snapshots
                    // otherwise, the snapshot is not valid (it was taken befor REX data updated or didn't change at all)
                    check(snapshot.after.total_assets != snapshot.before.total_assets, 
                        create_error_string2(
                            ERROR_US_2,
                            snapshot.after.total_assets.to_string(),
                            std::to_string((long)snapshot.id)
                        ).c_str()
                    );

                });
            }

            void action_update_last_snapshot() {
                PRINT("vapaee::wrap::global::action_update_last_snapshot() ------------------\n");
                require_auth(get_self());
                update_snapshot();
            }

            void action_init() {
                PRINT("vapaee::wrap::global::action_init() ------------------\n");

                globalstate gs_table(get_self(), get_self().value);
                auto itr = gs_table.begin();
                check(itr != gs_table.end(), ERROR_AI_1);

                gs_table.emplace(get_self(), [&](auto& state) {
                    state.liquidity = asset(0, vapaee::utils::SYS_TKN_SYMBOL);
                    state.current_shot = 0;
                    state.loans = 0;
                    // state.rex_balance = asset(0, vapaee::utils::REX_TKN_SYMBOL);
                    // state.matured_rex = asset(0, vapaee::utils::REX_TKN_SYMBOL);
                    state.participation.tlosv = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);
                    state.participation.tlosw = asset(0, vapaee::wrap::TLOSW_TKN_SYMBOL);
                });

                create_snapshot();

                action(
                    permission_level{get_self(), name("active")},
                    get_self(),
                    name("updateshot"),
                    std::make_tuple()
                ).send();
            }

            asset get_liquidity() {
                // PRINT("vapaee::wrap::global::get_liquidity()\n");
                return get_global_state().liquidity;
            }

            void add_liquidity(asset tlos) {
                // PRINT("vapaee::wrap::global::add_liquidity()\n");

                // check if the liquidity is valid
                check(tlos.symbol == vapaee::utils::SYS_TKN_SYMBOL, ERROR_ALI_1);

                globalstate gs_table(get_self(), get_self().value);
                auto itr = gs_table.begin();
                check(itr != gs_table.end(), ERROR_ALI_2);

                gs_table.modify(itr, same_payer, [&](auto& state) {
                    state.liquidity += tlos;
                });
            }
            
            void remove_liquidity(asset tlos) {
                PRINT("vapaee::wrap::global::remove_liquidity()\n");

                // check if the liquidity is valid
                check(tlos.symbol == vapaee::utils::SYS_TKN_SYMBOL, ERROR_RLI_1);

                globalstate gs_table(get_self(), get_self().value);
                auto itr = gs_table.begin();
                check(itr != gs_table.end(), ERROR_RLI_2);

                gs_table.modify(itr, same_payer, [&](auto& state) {
                    check(state.liquidity.amount > tlos.amount, ERROR_RLI_3);
                    state.liquidity -= tlos;
                });
            }

        };     
    };
};
*/