#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/wrap/tables.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/wrap/modules/rex.hpp>
#include <vapaee/pool/modules/liquidity.hpp>


namespace vapaee {
    namespace wrap {
        namespace voter {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            /*
                Para votar se usa el 
                "data": ⊖{
                    "proxy": "",
                    "producers": ⊖[
                        "argentinatls",
                        "caleosblocks",
                        "dailytelosbp",
                        "dappetizerbp",
                        "eosvenezuela",
                        "goodblocktls",
                        "monsterblock",
                        "swedencornet",
                        "teloscentral",
                        "telosmadrid1",
                        "telosmiamibp",
                        "telosuknodes",
                        "theteloscope",
                        "votetelosusa"
                    ],
                    "voter": "viterbotelos"
                },            
            */
            
            void vote_for_producers() {
                PRINT("vapaee::wrap::voter::vote_for_producers()");
                // TODO: VOTE. this is a temporary solution.
                // check(false, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
                action(
                    permission_level{get_self(), "active"_n},
                    vapaee::wrap::rexcontract,
                    vapaee::utils::check_name_from_string("voteproducer"),
                    std::make_tuple(
                        get_self(),
                        (name)0,
                        std::vector<name>{
                            vapaee::utils::check_name_from_string("caleosblocks"),
                            vapaee::utils::check_name_from_string("eosriobrazil"),
                            vapaee::utils::check_name_from_string("eostribeprod"),
                            vapaee::utils::check_name_from_string("goodblocktls"),
                            vapaee::utils::check_name_from_string("infinitybloc"),
                            vapaee::utils::check_name_from_string("kainosblkpro"),
                            vapaee::utils::check_name_from_string("kandaweather"),
                            vapaee::utils::check_name_from_string("persiantelos"),
                            vapaee::utils::check_name_from_string("southafrica1"),
                            vapaee::utils::check_name_from_string("teamgreymass"),
                            vapaee::utils::check_name_from_string("teleologytls"),
                            vapaee::utils::check_name_from_string("telosarabia1"),
                            vapaee::utils::check_name_from_string("telosglobal1"),
                            vapaee::utils::check_name_from_string("telosgreenbp"),
                            vapaee::utils::check_name_from_string("teloskitchen"),
                            vapaee::utils::check_name_from_string("telosmadrid1"),
                            vapaee::utils::check_name_from_string("telosuknodes"),
                            vapaee::utils::check_name_from_string("telosunlimit"),
                            vapaee::utils::check_name_from_string("theteloscope"),
                            vapaee::utils::check_name_from_string("theteloscrew"),
                            vapaee::utils::check_name_from_string("votetelosusa")
                        }
                    )
                ).send();
            }

            // init that calls vote_for_producers()
            void action_init() {
                PRINT("vapaee::wrap::voter::action_init()\n");
                vote_for_producers();
            }
        };     
    };
};
