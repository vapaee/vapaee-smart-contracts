#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/constants.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/global.hpp>

namespace vapaee {
    namespace dex {

        using namespace global;
        // using namespace utils;

        namespace experience {
            symbol POINTS_SYMBOL  = symbol(symbol_code("POINTS"),0);
            symbol EXP_SYMBOL     = symbol(symbol_code("EXP"),0);
            
            // experience ------------------------------------------------------------------
            void aux_make_user_rampayer(name owner) {
                PRINT("vapaee::dex::experience::aux_make_user_rampayer()\n");
                PRINT(" owner: ",owner.to_string()," \n");
                exp exptable(contract, contract.value);
                auto expptr = exptable.find(owner.value);
                if (expptr != exptable.end()) {
                    PRINT(" expptr != exptable.end() \n");
                    exptable.modify(*expptr, owner, [&](auto &a){
                        a.exp += asset(0, EXP_SYMBOL);
                    });                    
                } else {
                    PRINT(" expptr == exptable.end() \n");
                    exptable.emplace(owner, [&](auto &a){
                        a.owner = owner;
                        a.exp = asset(0, EXP_SYMBOL);
                    });
                }
                PRINT("vapaee::dex::experience::aux_make_user_rampayer() ...\n");
            }

            void action_reward_user(name user, const asset& points_reward, const asset& exp_reward) {
                PRINT("vapaee::dex::experience::action_reward_user()\n");
                PRINT(" user:          ",user.to_string()," \n");
                PRINT(" points_reward: ",points_reward.to_string()," \n");
                PRINT(" exp_reward:    ",exp_reward.to_string()," \n");

                check(has_auth(contract), ERROR_ARU_1);
                check(points_reward.symbol.code() == POINTS_SYMBOL.code(),
                    create_error_asset1(ERROR_ARU_2, points_reward).c_str());
                check(points_reward.symbol.precision() == POINTS_SYMBOL.precision(),
                    create_error_asset1(ERROR_ARU_3, points_reward).c_str());
                check(exp_reward.symbol.code() == EXP_SYMBOL.code(),
                    create_error_asset1(ERROR_ARU_4, exp_reward).c_str());
                check(exp_reward.symbol.precision() == EXP_SYMBOL.precision(),
                    create_error_asset1(ERROR_ARU_5, exp_reward).c_str());


                // get the current week number
                uint32_t week = get_current_week_number();


                // search for existing exp of the user
                // if exists, update it
                // if does not exist, create it
                exp exptable(contract, contract.value);
                auto ptr = exptable.find(user.value);
                if (ptr != exptable.end()) {
                    PRINT(" exp.modify() .exp += ",exp_reward.to_string(),"\n");
                    exptable.modify(*ptr, same_payer, [&](auto &a){
                        a.exp += exp_reward;
                    });                    
                } else {
                    PRINT(" exp.emplace() .exp = ",exp_reward.to_string(), " for ", user.to_string(),"\n");
                    exptable.emplace(contract, [&](auto &a){
                        a.owner = user;
                        a.exp = exp_reward;
                    });
                }

                // search for the latest week entry for this user (iteration)
                // does exist an entry for this week?
                // if so, update the number
                // if not, create a new one
                points pointstable(contract, contract.value);
                auto index = pointstable.get_index<name("owner")>();
                auto itr = index.lower_bound(user.value);
                for (itr = index.lower_bound(user.value); itr != index.end(); itr++) {
                    if (itr->owner == user) {
                        if (itr->week == week) {
                            break; // found
                        }
                    } else {
                        itr = index.end(); // not found
                        break;
                    }
                }

                if (itr != index.end()) {
                    PRINT(" exp.modify() .points += ",points_reward.to_string(),"\n");
                    pointstable.modify(*itr, same_payer, [&](auto &a){
                        a.points += points_reward;
                    });
                } else {
                    PRINT(" points.emplace() .points = ",points_reward.to_string(), " for ", user.to_string(),"\n");
                    pointstable.emplace(contract, [&](auto &a){
                        a.id = pointstable.available_primary_key();
                        a.week = week;
                        a.owner = user;
                        a.points = points_reward;
                    });
                }

                PRINT("vapaee::dex::experience::action_reward_user() ------------------------------ (end)\n");
            }
        };     
    };
};
