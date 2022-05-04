#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/wrap/tables.hpp>
// #include <vapaee/dex/tables.hpp>
#include <vapaee/wrap/modules/rex.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/experience.hpp>


namespace vapaee {
    namespace wrap {
        namespace maintenance {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            tuple<asset, asset, asset> get_maximum_rex_to_sell_from_loan(asset tlosv_quantity, uint64_t loan_id) {
                // get the loan
                loans table_loans(get_self(), get_self().value);
                auto loan = table_loans.find(loan_id);
                check(loan != table_loans.end(), create_error_id1(ERROR_GMRTSFL_1, loan_id).c_str());
                
                // get cancelation percent (how much of the debt is being canceled in percentage)
                double percent = (double) tlosv_quantity.amount / (double) loan->debt.amount;
                
                // callculate collateral
                asset rextosell = asset(loan->rex.amount * percent, loan->rex.symbol);

                // get max REX
                asset matured_rex = vapaee::wrap::rex::get_matured_rex();

                // for sheduling withdraw
                asset sheduling = asset(0, tlosv_quantity.symbol);
                if (rextosell.amount > matured_rex.amount) {
                    // if the rextosell is greater than the matured rex, then we truncate the rextosell to the matured rex
                    rextosell = matured_rex; // REX symbol
                    // we adjust what is the user actually being canceled (in TELOSV symbol)
                    uint64_t actual_tlosv_quantity = rextosell.amount * loan->debt.amount / loan->rex.amount;
                    // update sheduling because we need to shedule a future withdraw
                    sheduling.amount = tlosv_quantity.amount - actual_tlosv_quantity;
                    // update tlosv_quantity
                    tlosv_quantity.amount = actual_tlosv_quantity;
                }

                return make_tuple(rextosell, sheduling, tlosv_quantity);
            }

            void do_schedule_transfer(name to, asset quantity, string memo) {
                PRINT("vapaee::wrap::maintenance::do_schedule_transfer() ------------------\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo, "\n");
                
                // creates an entry in the transfers table
                transfers tbl(get_self(), get_self().value);
                tbl.emplace(get_self(), [&](auto& row) {
                    row.id = tbl.available_primary_key();
                    row.to = to;
                    row.quantity = quantity;
                    row.memo = memo;
                });
            }

            void do_schedule_withdraw(name to, asset quantity, uint64_t loan_id) {
                PRINT("vapaee::wrap::maintenance::do_schedule_withdraw() ------------------\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" loan_id: ", std::to_string((long) loan_id), "\n");

                // quantity is must be expressed in tlosv or tlosw
                check(
                    quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL ||
                    quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL
                , create_error_asset1(ERROR_DSW_1, quantity).c_str());
                
                // creates an entry in the withdraws table
                withdraws tbl(get_self(), get_self().value);
                tbl.emplace(get_self(), [&](auto& row) {
                    row.id = tbl.available_primary_key();
                    row.to = to;
                    row.loan_id = loan_id;
                    row.quantity = quantity;
                });
            }

            void do_transfer(name to, asset quantity, string memo) {
                PRINT("vapaee::wrap::maintenance::do_transfer() ------------------\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo, "\n");

                // transfer the token
                action(
                    permission_level{get_self(), name("active")},
                    vapaee::dex::utils::get_contract_for_token(quantity.symbol.code()),
                    name("transfer"),
                    std::make_tuple(get_self(), to, quantity, memo)
                ).send();
            }

            bool do_withdraw(name user, asset quantity, uint64_t loan_id, uint64_t w_id) {
                PRINT("vapaee::wrap::maintenance::do_withdraw() ------------------\n");
                PRINT(" user: ", user.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" loan_id: ", std::to_string((long) loan_id), "\n");
                PRINT(" w_id: ", std::to_string((long) w_id), "\n");

                withdraws w_table(get_self(), get_self().value);
                auto itr = w_table.find(w_id);
                bool done = false;
                
                // get matured_rex
                asset matured_rex = vapaee::wrap::rex::get_matured_rex();
                asset actual;

                // quantity is must be expressed in tlosv or tlosw
                check(
                    quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL ||
                    quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL
                , create_error_asset1(ERROR_DSW_1, quantity).c_str());
                
                // if there's not matured REX, then do nothing
                if (matured_rex.amount == 0) {
                    PRINT("vapaee::wrap::maintenance::do_withdraw() -> matured_rex.amount == 0\n");
                    return done;
                }

                // decide what kind of withdrawal are we performing
                if (quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL) {
                    // user is CANCELING a loan so we call the cancel action with user params

                    // get the maximum amount of REX that can be sold
                    auto result = vapaee::wrap::maintenance::get_maximum_rex_to_sell_from_loan(quantity, loan_id);
                    asset sheduling = std::get<1>(result);
                    actual = std::get<2>(result);

                    if (sheduling.amount > 0) {
                        // update withdraw entry
                        w_table.modify(itr, get_self(), [&](auto& row) {
                            row.quantity.amount -= actual.amount;
                        });
                    } else {
                        // remove withdraw entry
                        w_table.erase(itr);
                    }

                    action(
                        permission_level{get_self(), name("active")},
                        get_self(),
                        name("cancel"),
                        std::make_tuple(user, actual, loan_id)
                    ).send();

                    done = true;

                } else {
                    // user is REDEEMING some REX

                    // calculated actual withdraw amount
                    if (quantity.amount > matured_rex.amount) {
                        // take all matured rex and update withdraw entry
                        actual = matured_rex;
                        w_table.modify(itr, get_self(), [&](auto& row) {
                            row.quantity.amount -= actual.amount;
                        });
                    } else {
                        // take all quantity and delete withdraw from table
                        actual = quantity;
                        w_table.erase(itr);
                    }

                    // user is redeeming a loan so we call the redeem action with user params
                    action(
                        permission_level{get_self(), name("active")},
                        get_self(),
                        name("redeem"),
                        std::make_tuple(user, actual)
                    ).send();

                    done = false;
                }
                
                return done;
            }

            string aux_maintenance_from_send_transfer(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_send_transfer()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

                string report = "";
                transfers tbl(get_self(), get_self().value);
                auto transfer = tbl.begin();
                if (transfer != tbl.end()) {
                    PRINT(" -> sending transfer: ", transfer->to.to_string(), " ", transfer->quantity.to_string(), " ", transfer->memo, "\n");
                    do_transfer(transfer->to, transfer->quantity, transfer->memo);
                    tbl.erase(transfer);
                
                    state gconf = vapaee::dex::global::get();
                    exp.amount += gconf.maint_reward_update_loans_exp;
                    pts.amount += gconf.maint_reward_update_loans_pts;
                    report += string("|deferred-transfer:") + transfer->to.to_string() + "," + transfer->quantity.to_string();
                }

                return report;
            }

            string aux_maintenance_from_withdraws(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_send_withdraw()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

                string report = "";
                withdraws tbl(get_self(), get_self().value);
                auto withdraw = tbl.begin();
                if (withdraw != tbl.end()) {
                    PRINT(" -> sending withdraw: ", withdraw->to.to_string(), " ", withdraw->quantity.to_string(), " \n");
                    bool done = do_withdraw(withdraw->to, withdraw->quantity, withdraw->loan_id, withdraw->id);
                    state gconf = vapaee::dex::global::get();
                    if (done) {
                        // extra points
                        exp.amount += gconf.maint_reward_update_loans_exp;
                        pts.amount += gconf.maint_reward_update_loans_pts;
                    }
                
                    exp.amount += gconf.maint_reward_update_loans_exp;
                    pts.amount += gconf.maint_reward_update_loans_pts;
                    report += string("|deferred-withdraw:")+ withdraw->to.to_string() + "," + withdraw->quantity.to_string();
                }

                return report;
            }

            void action_do_maintenance(name credits_to) {
                PRINT("vapaee::wrap::maintenance::action_do_maintenance() ------------------\n");
                PRINT(" credits_to: ",credits_to.to_string()," \n");

                string report;
                asset points = asset(0, vapaee::dex::experience::POINTS_SYMBOL);
                asset exp = asset(0, vapaee::dex::experience::EXP_SYMBOL);

                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: transfers     --\n");
                report += aux_maintenance_from_send_transfer(points, exp);

                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: transfers     --\n");
                report += aux_maintenance_from_withdraws(points, exp);


                // TODO: hay que crear una lista first-in-first-out de gente que quiere redimir sus TLOSW y no hay liquidez
                // el mantenimiento debería fijarse si hay liquidez y satizfacer todas las demandas que se pueda (con un límite).

                if (points.amount > 0) {
                    action(
                        permission_level{contract,name("active")},
                        vapaee::dex::contract,
                        name("reward"),
                        std::make_tuple(credits_to, points, exp)
                    ).send();
                }

            }

        };     
    };
};
