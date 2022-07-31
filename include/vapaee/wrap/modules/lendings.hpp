#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/utils.hpp>
// #include <vapaee/wrap/modules/global.hpp>
#include <vapaee/wrap/modules/maintenance.hpp>
#include <vapaee/wrap/modules/rex.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/wrap/tables.hpp>
                      

namespace vapaee {
    namespace wrap {
        namespace lendings {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            /**
            * @brief This functions takes a loan which is up to date and is being canceled (TLOSV debt).
            *        It actually cancels the loan and returns the collateral to the owner plus REX farming.
            *        It also update de dates, and decreases the debt. If the debt is zero, it deletes the loan.
            *        User --(TLOSV)--> TelosWrapper --(TLOS)--> User
            * 
            * @param loan_id id of a loan which is being canceled and is up to date.
            */
            /*void action_cancel_uptodate_loan(uint64_t loan_id) {
                PRINT("vapaee::wrap::lendings::action_cancel_uptodate_loan()\n");
                PRINT(" loan_id: ", std::to_string((unsigned long)loan_id), "\n");

                // get the loan and check is actually up to date and being canceled
                loans loans_table(get_self(), get_self().value);
                auto loan = loans_table.find(loan_id);
                check(loan != loans_table.end(), create_error_id1(ERROR_ACL_1, loan_id).c_str());
                check(loan->canceling.tlosv.amount > 0, create_error_asset1(ERROR_ACL_2, loan->canceling.tlosv).c_str());
                check(loan->last_shot == vapaee::wrap::global::get_current_shot(), create_error_id1(ERROR_ACL_3, loan->last_shot).c_str());

                // get cancelation percent (how much of the debt is being canceled in percentage)
                double percent = (double) loan->canceling.tlosv.amount / (double) loan->debt.tlosv.amount;
                
                // callculate collateral
                asset collateral = asset(loan->profits.tlosv.amount * percent, loan->profits.tlosv.symbol);

                // send collateral to the owner
                string memo = string("TelosWrapper: canceling loan ") + std::to_string((unsigned long)loan_id);
                action(
                    permission_level{contract, "active"_n},
                    vapaee::dex::utils::get_contract_for_token(collateral.symbol.code()),
                    "transfer"_n,
                    make_tuple(
                        contract, loan->owner, collateral, memo)
                ).send();

                // update loan
                loans_table.modify(loan, get_self(), [&](auto& l) {
                    l.debt.tlosv.amount -= l.canceling.tlosv.amount;
                    l.canceling.tlosv = asset(0, l.canceling.tlosv.symbol);
                    l.updated_at = vapaee::dex::global::get_now_time_point_sec();
                });

                PRINT("vapaee::wrap::lendings::action_cancel_uptodate_loan()...\n");
            }*/

            /**
            * @brief .....
            *        User --(TLOSW)--> TelosWrapper --(TLOS)--> User
            * 
            * @param loan_id id of a loan which is being canceled and is up to date.
            */
            /*void action_redeem_uptodate_loan(uint64_t loan_id) {
                PRINT("vapaee::wrap::lendings::action_redeem_uptodate_loan()\n");
                PRINT(" loan_id: ", std::to_string((unsigned long)loan_id), "\n");

                // TODO: action_redeem_uptodate_loan implementar
                // hay que hacer lo mismo que para TLOSV sólo que en vez de mandarle los TLOS +farming, mandar solo el farming

                

                PRINT("vapaee::wrap::lendings::action_redeem_uptodate_loan()...\n");
            }*/

            /*void push_update_loan(uint64_t loan_id) {
                PRINT("vapaee::wrap::lendings::push_update_loan()\n");
                PRINT(" loan_id: ", std::to_string((unsigned long)loan_id), "\n");

                action(
                    permission_level{contract, "active"_n},
                    get_self(),
                    "updateloan"_n,
                    make_tuple(loan_id)
                ).send();
                
            }*/

            /*void add_to_loan_canceling(uint64_t loan_id, asset tlosv, asset tlosw) {
                PRINT("vapaee::wrap::lendings::add_to_loan_canceling()\n");
                PRINT(" loan_id: ", std::to_string((unsigned long)loan_id), "\n");
                PRINT(" tlosv: ", tlosv.to_string(), "\n");
                PRINT(" tlosw: ", tlosw.to_string(), "\n");

                // get the loan
                loans loans_table(get_self(), get_self().value);
                auto loan = loans_table.find(loan_id);
                check(loan != loans_table.end(), create_error_id1(ERROR_ATLC_1, loan_id).c_str());

                // update the loan
                loans_table.modify(loan, same_payer, [&](auto& l) {
                    l.canceling.tlosv.amount += tlosv.amount;
                    l.canceling.tlosw.amount += tlosw.amount;
                    if (tlosv.amount > 0) {
                        loan.updated_at = vapaee::dex::global::get_now_time_point_sec();
                    }
                    if (tlosw.amount > 0) {
                        loan.redeemed_at = vapaee::dex::global::get_now_time_point_sec();
                    }
                });

                // push the loan to update it si it can be canceled
                push_update_loan(loan_id);

                PRINT("vapaee::wrap::lendings::add_to_loan_canceling()...\n");
            }*/

            void handle_cancel_user_loan(name user, asset tlosv_quantity, uint64_t loan_id) {
                PRINT("vapaee::wrap::lendings::handle_cancel_user_loan()\n");
                PRINT(" user: ", user.to_string(), "\n");
                PRINT(" tlosv_quantity: ", tlosv_quantity.to_string(), "\n");
                PRINT(" loan_id: ", std::to_string((unsigned long)loan_id), "\n");

                // -------------
                // user can only cancel a loan if the tlosv_quantity is TLOSV
                check(tlosv_quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, create_error_asset1(ERROR_HCUL_1, tlosv_quantity).c_str());

                // get the loan
                loans loans_table(get_self(), get_self().value);
                auto loan = loans_table.find(loan_id);
                check(loan != loans_table.end(), create_error_id1(ERROR_HCUL_2, loan_id).c_str());

                // check if the user is the owner of the loan
                check(user == loan->owner, create_error_name2(ERROR_HCUL_3, user, loan->owner).c_str());
                
                // tlosv_quantity must be lower than the debt.tlosv - canceling.tlosv
                check(tlosv_quantity.amount <= loan->debt.amount, 
                    create_error_asset2(ERROR_HCUL_3, tlosv_quantity, loan->debt).c_str());

                // check the loan has been created at least N days ago 
                // TODO: this must be part of a global config
                int days_locked = 10;
                time_point_sec now = vapaee::dex::global::get_now_time_point_sec();
                time_point_sec end_time = vapaee::dex::global::get_N_days_from_point_sec(loan->created_at, days_locked);
                check(now < end_time, create_error_id3(
                    ERROR_HCUL_4, loan_id, (uint64_t)end_time.sec_since_epoch(), (uint64_t)now.sec_since_epoch()).c_str());

                // get the maximum amount of REX that can be sold (acording to matured REX)
                auto result = vapaee::wrap::maintenance::get_maximum_rex_to_sell_from_loan(tlosv_quantity, loan_id);
                asset rextosell = std::get<0>(result);
                asset sheduling = std::get<1>(result);
                tlosv_quantity  = std::get<2>(result);

                // we do actually withdraw from REX
                asset withdrawal_tlos = vapaee::wrap::rex::withdraw_REX(rextosell);

                // calculate fees
                // TODO: this must be part of a global config
                double fee_percent = 0.1; 
                
                uint64_t gains = withdrawal_tlos.amount - tlosv_quantity.amount;
                uint64_t fees = gains * fee_percent;
                asset collateral = asset(withdrawal_tlos.amount - fees, withdrawal_tlos.symbol);
                asset profits = asset(fees, collateral.symbol);

                // send collateral to the owner
                string memo = string("TelosWrapper: canceling loan ") + std::to_string((unsigned long)loan_id) + 
                              " (fee: " + profits.to_string() + ")";
                action(
                    permission_level{contract, "active"_n},
                    vapaee::dex::utils::get_contract_for_token(collateral.symbol.code()),
                    "transfer"_n,
                    make_tuple(
                        vapaee::wrap::contract, user, collateral, memo)
                ).send();

                // handle profits to self
                action(
                    permission_level{contract, "active"_n},
                    get_self(),
                    "profits"_n,
                    make_tuple(profits)
                ).send();

                // if user canceled the total debt then erase loan entry, else update the loan
                if (tlosv_quantity.amount == loan->debt.amount) {
                    loans_table.erase(loan);
                    check(sheduling.amount == 0, create_error_asset1(ERROR_HCUL_5, sheduling).c_str());
                } else {
                    loans_table.modify(loan, same_payer, [&](auto& l) {
                        l.debt.amount -= tlosv_quantity.amount;
                        l.rex.amount -= rextosell.amount;
                        // l.updated_at = vapaee::dex::global::get_now_time_point_sec();
                    });
                }

                if (sheduling.amount > 0) {
                    // schedule a withdraw
                    vapaee::wrap::maintenance::do_schedule_withdraw(user, sheduling, loan_id);
                }

            }

            /*void deposit_tlos_into_rex(asset quantity) {
                PRINT("vapaee::wrap::lendings::deposit_tlos_into_rex()\n");
                PRINT(" quantity: ", tokens.to_string(), "\n");

                // find the current last snapshot
                // uint64_t last_shot = vapaee::wrap::global::get_current_shot();
                // snapshot_table last = vapaee::wrap::global::get_snapshot(last_shot);

                // create a new snapshot
                // vapaee::wrap::global::create_snapshot();

                // deposit quantity into rex (sending "deposit" and "buyrex" to REX account (eosio))
                

                // take a snapshot and fill the after status of the new snapshot
                // action(
                //     permission_level{get_self(), name("active")},
                //     get_self(),
                //     name("updateshot"),
                //     std::make_tuple()
                // ).send();

                PRINT("vapaee::wrap::lendings::deposit_tlos_into_rex()...\n");
            }*/

            asset issue_and_send_tlosv_to_user(name user, asset tlos_quantity, uint64_t loan_id) {
                PRINT("vapaee::wrap::lendings::issue_and_send_tlosv_to_user()\n");
                PRINT(" user: ", user.to_string(), "\n");
                PRINT(" tlos_quantity: ", tlos_quantity.to_string(), "\n");
                PRINT(" loan_id: ", std::to_string((unsigned long)loan_id), "\n");

                // issue DEX tokens to user
                asset dex_quantity = asset(tlos_quantity.amount, vapaee::wrap::TLOSV_TKN_SYMBOL);
                
                action(
                    permission_level{get_self(), name("active")},
                    vapaee::token::contract,
                    name("issue"),
                    std::make_tuple(get_self(), dex_quantity, 
                        std::string("issued to user: ")
                        .append(user.to_string())
                        .append(", loan: ")
                        .append(std::to_string((unsigned long)loan_id))
                    )
                ).send();
                
                // send DEX token to user
                
                action(
                    permission_level{get_self(), name("active")},
                    vapaee::token::contract,
                    name("transfer"),
                    std::make_tuple(get_self(), user, dex_quantity, 
                        std::string("TelosWrapper - ")
                        .append("transforming ")
                        .append(tlos_quantity.to_string())
                        .append(" into ")
                        .append(dex_quantity.to_string())
                        .append(" (loan id:")
                        .append(std::to_string((unsigned long)loan_id))
                        .append(")")
                    )
                ).send();
                

                PRINT("vapaee::wrap::lendings::issue_and_send_tlosv_to_user()...\n");

                return dex_quantity;
            }

            uint64_t get_empty_loan_id_for_user(name user) {
                PRINT("vapaee::wrap::lendings::get_empty_loan_id_for_user()\n");
                PRINT(" user: ", user.to_string(), "\n");

                // init var
                uint64_t loan_id = 0;

                loans loans_table(get_self(), get_self().value);

                auto owner_index = loans_table.get_index<name("owner")>();
                auto loan_ptr = owner_index.begin();
                for (; loan_ptr != owner_index.end(); loan_ptr++) {
                    if (loan_ptr->owner == user && loan_ptr->debt.amount == 0) {
                        break;
                    }
                }

                check(loan_ptr != owner_index.end(), create_error_name1(ERROR_GELIUFU_1, user).c_str());

                return loan_ptr->id;
            }

            void handle_loan(name from, asset quantity) {
                PRINT("vapaee::wrap::lendings::handle_loan()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                // PRINT(" telosv_quantity: ", telosv_quantity.to_string(), "\n");

                // check if the user has sent more TLOS than is askin for TLOSV
                check(quantity.symbol == vapaee::utils::SYS_TKN_SYMBOL, 
                    create_error_asset1(ERROR_HL_1, quantity).c_str());
                // check(telosv_quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                //     create_error_asset1(ERROR_HL_2, telosv_quantity).c_str());
                // check(quantity.amount >= telosv_quantity.amount, 
                //     create_error_asset2(ERROR_HL_3, quantity, telosv_quantity).c_str());

                
                // get the loan previously created using the user's RAM
                uint64_t loan_id = get_empty_loan_id_for_user(from);

                
                // isssue TLOSV and TLOSW and send to user
                // asset telosw_quantity = asset(quantity.amount - telosv_quantity.amount, vapaee::wrap::TLOSW_TKN_SYMBOL);
                // tloswrap tokens = tloswrap(telosv_quantity, telosw_quantity);

                // TODO: avoid infinite leverage
                // check(telosw_quantity <= quantity * allowed_PERCENT, "You need to ask for more TLOSV (less TLOSW)");

                // tloswrap tokens;
                // tokens.tlosv = telosv_quantity;
                // tokens.tlosw = telosw_quantity;
                asset dex_quantity = issue_and_send_tlosv_to_user(from, quantity, loan_id);

                // put the TLOS of the user to work in REX. This will take a snapshot before loan is added
                // the will auto send an action_updateshot() to update the snapshot.after state
                asset rex_quantity = vapaee::wrap::rex::deposit_tlos_to_rex(quantity);

                // get the loan
                loans loans_table(get_self(), get_self().value);
                auto loan = loans_table.find(loan_id);

                // update loan
                loans_table.modify(loan, get_self(), [&](auto& l) {
                    l.debt = dex_quantity;
                    l.rex = rex_quantity;
                });

                PRINT("vapaee::wrap::lendings::handle_loan()...\n");
            }

            // wrap TLOSW functions ----------------
            /*asset handle_redeem(name from, asset tlosw) {
                PRINT("vapaee::wrap::handler::handle_redeem()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" tlosw: ", tlosw.to_string(), "\n");

                // TODO: aplicar la restricción de que el usuario puede redimir cuando quiera pero se le devolverán los TLOS efectivamente
                // una vez cumplido el plazo del REX de 5 días locked

                // check if tlosw is in TLOSW
                check(tlosw.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL, 
                    create_error_asset1(ERROR_HR_1, tlosw).c_str());                
                
                asset remaining = tlosw;

                // find the current last snapshot
                uint64_t last_shot = vapaee::wrap::global::get_current_shot();
                snapshot_table last = vapaee::wrap::global::get_snapshot(last_shot);

                // create a new snapshot
                vapaee::wrap::global::create_snapshot();

                // TODO: handle_redeem
                // 1 - checkear cuantos TLOS tengo maduros en el REX y a partir de saber eso sabemos el máximo de TLOSW que puedo redimir
                asset max_redeem   = vapaee::wrap::rex::get_max_liquidity();
                asset max_withdraw = vapaee::wrap::rex::get_max_withdraw();

                // ghather how much we will need to wthdraw from the REX
                asset rex_withdraw = asset(0, vapaee::utils::SYS_TKN_SYMBOL);
                
                // 2 - iterate over the user's loans and cancelate the ones with debt.tlosw > 0
                loans loans_table(get_self(), get_self().value);
                auto owner_index = loans_table.get_index<name("owner")>();
                auto loan_ptr = owner_index.lower_bound(from.value);
                
                for (;loan_ptr != owner_index.end() && loan_ptr->owner == from && remaining.amount > 0; loan_ptr++) {
                    // if the loan has no debt, then skip it
                    if (loan_ptr->debt.tlosw.amount == 0) continue;

                    // if the loan is already canceling, check if is there's enough room to add canceling amount
                    if (loan_ptr->debt.tlosw.amount - loan_ptr->canceling.tlosw.amount == 0) continue;
                    
                    // max we can cancel here is debt.tlosw
                    asset cancel = loan_ptr->debt.tlosw;

                    // but if there's already a canceling amount, then we can cancel less
                    if (loan_ptr->canceling.tlosw.amount > 0) {
                        cancel.amount -= loan_ptr->canceling.tlosw.amount;
                    }

                    // if the amount we are canceling is grather than we need then we get smaller
                    if (cancel.amount > remaining.amount) {
                        cancel.amount = remaining.amount;
                    }

                    // finally, if the amount we are canceling is grather than the max we can cancel, then we get smaller
                    if (rex_withdraw.amount + cancel.amount > max_withdraw.amount) {
                        cancel.amount = max_withdraw.amount - rex_withdraw.amount;
                    }

                    // we update variables
                    remaining.amount -= cancel.amount;
                    rex_withdraw.amount += cancel.amount;
                    
                    if (cancel.amount > 0) {
                        // update the canceling amount
                        add_to_loan_canceling(loan_ptr->id, asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL), cancel);
                    }
                }

                // 3 - iterar sobre las loans que tienen el redeemed_at más viejo, cancelarlas y actualizar el redeemed_at
                auto redeemed_index = loans_table.get_index<name("redeemed")>();
                auto other_loan = redeemed_index.lower_bound(0);

                for (
                    ; // separator
                    other_loan != redeemed_index.end() &&
                    other_loan->owner != from &&
                    remaining.amount > 0 && &&
                    rex_withdraw.amount < max_withdraw.amount
                    ; // separator
                    
                ) {
                    // if the loan has no debt, then skip it
                    if (other_loan->debt.tlosw.amount == 0) continue;

                    // if the loan is already canceling, check if is there's enough room to add canceling amount
                    if (other_loan->debt.tlosw.amount - other_loan->canceling.tlosw.amount == 0) continue;
                    
                    // max we can cancel here is debt.tlosw
                    asset cancel = other_loan->debt.tlosw;

                    // but if there's already a canceling amount, then we can cancel less
                    if (other_loan->canceling.tlosw.amount > 0) {
                        cancel.amount -= other_loan->canceling.tlosw.amount;
                    }

                    // if the amount we are canceling is grather than we need then we get smaller
                    if (cancel.amount > remaining.amount) {
                        cancel.amount = remaining.amount;
                    }

                    // finally, if the amount we are canceling is grather than the max we can cancel, then we get smaller
                    if (rex_withdraw.amount + cancel.amount > max_withdraw.amount) {
                        cancel.amount = max_withdraw.amount - rex_withdraw.amount;
                    }

                    // we update variables
                    remaining.amount -= cancel.amount;
                    rex_withdraw.amount += cancel.amount;
                    
                    auto aux = other_loan;
                    other_loan++;

                    if (cancel.amount > 0) {
                        // update the canceling amount
                        add_to_loan_canceling(aux->id, asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL), cancel);
                    }
                }

                // 4 - withdraw TLOS from the REX
                vapaee::wrap::rex::withdraw_from_rex(rex_withdraw);

                // 5 - send TLOS to the user
                asset actual_redeem = asset(tlosw.amount - remaining.amount, vapaee::utils::SYS_TKN_SYMBOL);

                action(
                    permission_level{get_self(), name("active")},
                    vapaee::dex::utils::get_contract_for_token(actual_redeem.symbol.code()),,
                    name("transfer"),
                    std::make_tuple(
                        get_self(),
                        from,
                        actual_redeem,
                        std::string("you redeemed ") + actual_redeem.to_string()
                    )
                ).send();

                if (remaining.amount > 0) {
                    // if we counln't redeem all TLOSW then we send back the remaining to the user
                    action(
                        permission_level{get_self(), name("active")},
                        vapaee::dex::utils::get_contract_for_token(remaining.symbol.code()),
                        name("transfer"),
                        std::make_tuple(
                            get_self(),
                            from,
                            remaining,
                            std::string("It wasn't possible to redeem all TLOSW you sent because we ran out of TLOS liquidity")
                        )
                    ).send();
                }

                // take a snapshot and fill the after status of the new snapshot
                action(
                    permission_level{get_self(), name("active")},
                    get_self(),
                    name("updateshot"),
                    std::make_tuple()
                ).send();

                return remaining;
            }*/

            void handle_redeem(name user, asset quantity_rex) {
                PRINT("vapaee::wrap::handler::handle_redeem()\n");
                PRINT(" user: ", user.to_string(), "\n");
                PRINT(" quantity_rex: ", quantity_rex.to_string(), "\n");

                // check if quantity_rex is in quantity_rex
                check(quantity_rex.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                    create_error_asset1(ERROR_HR_1, quantity_rex).c_str());

                asset sent = quantity_rex;
                
                // 1 - get matured REX
                asset matured_rex = vapaee::wrap::rex::get_matured_rex();

                // 2 - truncate to max withdraw
                asset schedule = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);
                if (quantity_rex.amount > matured_rex.amount) {
                    // we need to schedule the remaining
                    schedule.amount = quantity_rex.amount - matured_rex.amount;
                    
                    // update the amount redeemeable
                    quantity_rex.amount = matured_rex.amount;
                }

                // 3 - withdraw from REX
                asset withdrawal = vapaee::wrap::rex::withdraw_REX(quantity_rex);

                // prepare memo
                string memo = std::string("you redeemed ") + sent.to_string() + " getting " + withdrawal.to_string();

                if (schedule.amount > 0) {
                    memo = std::string("you tried to redeem ") + sent.to_string() + " but at the moment we only have " + matured_rex.to_string() + " in REX. So we scheduled " + schedule.to_string() + " to be redeemed later";

                    vapaee::wrap::maintenance::do_schedule_withdraw(user, schedule, 0);
                }

                // 4 - send TLOS to the user
                action(
                    permission_level{get_self(), name("active")},
                    vapaee::dex::utils::get_contract_for_token(withdrawal.symbol.code()),
                    name("transfer"),
                    std::make_tuple(
                        get_self(),
                        user,
                        withdrawal,
                        memo
                    )
                ).send();

                // 5 - burn the quantity_rex                
                action(
                    permission_level{get_self(), "active"_n},
                    vapaee::token::contract,
                    "burn"_n,
                    std::make_tuple(quantity_rex)
                ).send();
                
                
            }            

            /*void handle_change_debt(name user, asset quantity, uint64_t loan_id) {
                PRINT("vapaee::wrap::handler::handle_change_debt()\n");
                PRINT(" user: ", user.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" loan_id: ", std::to_string((unsigned long)loan_id), "\n");

                // check if quantity is in TLOSW or TLOSV
                check(quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL || quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                    create_error_asset1(ERROR_HCD_1, quantity).c_str());
                
                // TODO: handle_change_debt
                // 1 - encontrar la loan usando loan_id
                // 2 - verificar que el usuario sea el owner
                // 3 - verificar que la loan no se esté cancelando
                // 4 - verificar que la cantidad de debt.tlosX sea mayor o igual a quantity

                // 5 - actualizar el debt.tlosX -= quantity
                // 6 - actualizar el debt.tlosY += quantity
                // 7 - burn quantityX
                // 8 - issue quantityY
                // 9 - send quantityY to user
                // 10 - update loan updated_at

            }*/

            
            /**
            * @brief This function take the oldest loan and transform some TLOSV to TLOSW updating the updated_at field.
            * @param tlosv The account to issue tokens to.
            * @return actual amount of tlosv spent
            */            
            /*asset delegate_oldest_loan(asset tlosv) {
                PRINT("vapaee::wrap::handler::delegate_oldest_loan()\n");
                PRINT(" tlosv: ", tlosv.to_string(), "\n");

                // check tlosv symbol is TLOSV
                check(tlosv.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, 
                    create_error_asset1(ERROR_AOL_1, tlosv).c_str());

                // find the oldest loan
                loans loans_table(get_self(), get_self().value);
                auto updated_index = loans_table.get_index<name("updated")>();
                auto loan_ptr = updated_index.lower_bound(0);

                // if no found (weird case) return false
                if (loan_ptr == updated_index.end()) return tlosv;

                // gather what we rwally spend (return value)
                asset spent = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);

                // iterate while not spent all tlosv. Max 3 iterations
                uint64_t i = 0;
                while (i++ <= 3 && tlosv.amount > 0) {
                    loan_ptr = updated_index.lower_bound(0);

                    // if there is some debt.tlosv we age up to tlosv
                    if (loan_ptr->debt.tlosv.amount > 0) {
                        asset spend = loan_ptr->debt.tlosv;
                        if (loan_ptr->debt.tlosv.amount > tlosv.amount) {
                            spend.amount = tlosv.amount;
                        }

                        // update tlosv.amount
                        tlosv.amount -= spend.amount;
                        spent.amount += spend.amount;

                        // update the loan
                        updated_index.modify(loan_ptr, get_self(), [&](auto& loan) {
                            loan.debt.tlosv.amount -= spend.amount;
                            loan.debt.tlosw.amount += spend.amount;
                            loan.updated_at = vapaee::dex::global::get_now_time_point_sec();
                            loan.redeemed_at = loan.created_at; // this make it available right away for TLOSW redemption
                        });                        
                    }
                }

                return spent;
            }*/
            // -------------------------------------



            // this action allocates user's RAM for a new loan
            void action_init_loan_ram(name owner) {
                PRINT("vapaee::wrap::lendings::action_init_loan_ram() ------------------\n");
                PRINT(" owner: ", owner.to_string(), "\n");


                // get the loan table
                loans loans_table(get_self(), get_self().value);

                // create a new loan
                uint64_t loan_id = loans_table.available_primary_key();
                loans_table.emplace(get_self(), [&](auto& loan) {
                    // loan ids
                    loan.id = loan_id;
                    loan.owner = owner;

                    // debt splitted in two parts: TLOSV and TLOSW
                    // loan.debt.tlosv = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);
                    // loan.debt.tlosw = asset(0, vapaee::wrap::TLOSW_TKN_SYMBOL);
                    loan.debt = asset(0, vapaee::utils::SYS_TKN_SYMBOL);
                    loan.rex = asset(0, vapaee::utils::REX_TKN_SYMBOL);

                    // initializing in zero the canceling debt
                    // loan.canceling.tlosv = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);
                    // loan.canceling.tlosw = asset(0, vapaee::wrap::TLOSW_TKN_SYMBOL);

                    // setting dates
                    loan.created_at = vapaee::dex::global::get_now_time_point_sec();
                    // loan.updated_at = vapaee::dex::global::get_now_time_point_sec();
                    // loan.redeemed_at = vapaee::dex::global::get_now_time_point_sec();

                    // init profits data
                    // loan.last_shot = vapaee::wrap::global::get_current_shot();
                    // loan.profits = asset(0, vapaee::utils::SYS_TKN_SYMBOL);
                });




            }

        };     
    };
};


