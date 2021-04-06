#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/global.hpp>

using namespace eosio;

namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace client {
            
            void aux_assert_client_is_valid(uint64_t client) {
                PRINT("vapaee::dex::client::aux_assert_client_is_valid()\n");
                PRINT(" client: ", std::to_string((long unsigned) client), "\n");

                clients clitable(contract, contract.value);
                auto ptr = clitable.find(client);
                check(ptr != clitable.end(), create_error_id1(ERROR_AACIV_1, client).c_str());

                PRINT("vapaee::dex::client::aux_assert_client_is_valid() ...\n");
            }

            void action_add_client(name admin, name receiver, string params, string title, string website, string brief, string banner, string thumbnail) {
                PRINT("vapaee::dex::client::action_add_client()\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" receiver: ", receiver.to_string(), "\n");
                PRINT(" params: ", params.c_str(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" website: ", website.c_str(), "\n");
                PRINT(" brief: ", brief.c_str(), "\n");
                PRINT(" banner: ", banner.c_str(), "\n");
                PRINT(" thumbnail: ", thumbnail.c_str(), "\n");

                // receiver and admin must exist
                check( is_account( receiver ), create_error_name1(ERROR_AAC_1, receiver).c_str());
                check( is_account( admin ), create_error_name1(ERROR_AAC_2, admin).c_str());

                // signature and ram payer
                name rampayer = admin;
                if (has_auth(contract)) {
                    rampayer = contract;
                } else {
                    check(has_auth(admin), create_error_name1(ERROR_AAC_3, admin).c_str());
                }

                clients clitable(vapaee::dex::contract, contract.value);
                
                uint64_t client = clitable.available_primary_key();
                clitable.emplace(rampayer, [&]( auto& a ){
                    a.id        = client;
                    a.admin     = admin;
                    a.receiver  = receiver;
                    a.params    = params;
                    a.title     = title;
                    a.website   = website;
                    a.brief     = brief;
                    a.banner    = banner;
                    a.thumbnail = thumbnail;
                    a.date      = vapaee::dex::global::get_now_time_point_sec();
                });

                PRINT(" -> emplace: ", receiver.to_string(), " with id ", std::to_string((unsigned) client), "\n");

                PRINT("vapaee::dex::client::action_add_client() ...\n");
            }

            void action_update_client(uint64_t client, name admin, name receiver, string params, string title, string website, string brief, string banner, string thumbnail) {
                PRINT("vapaee::dex::client::action_add_client()\n");
                PRINT(" client: ", std::to_string((unsigned) client), "\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" receiver: ", receiver.to_string(), "\n");
                PRINT(" params: ", params.c_str(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" website: ", website.c_str(), "\n");
                PRINT(" brief: ", brief.c_str(), "\n");
                PRINT(" banner: ", banner.c_str(), "\n");
                PRINT(" thumbnail: ", thumbnail.c_str(), "\n");

                // receiver and admin must exist
                check( is_account( receiver ), create_error_name1(ERROR_AUC_1, receiver).c_str());
                check( is_account( admin ), create_error_name1(ERROR_AUC_2, admin).c_str());

                // signature and ram payer
                name rampayer = admin;
                if (has_auth(contract)) {
                    rampayer = same_payer;
                } else {
                    check(has_auth(admin), create_error_name1(ERROR_AUC_3, admin).c_str());
                }

                // check tuple existance
                aux_assert_client_is_valid(client);
                
                // update table
                clients clitable(contract, contract.value);
                auto ptr = clitable.find(client);
                clitable.modify( *ptr, rampayer, [&](auto & a){
                    a.admin     = admin;
                    a.receiver  = receiver;
                    a.params    = params;
                    a.title     = title;
                    a.website   = website;
                    a.brief     = brief;
                    a.banner    = banner;
                    a.thumbnail = thumbnail;
                    a.date      = vapaee::dex::global::get_now_time_point_sec();
                });

                PRINT(" -> modify: ", receiver.to_string(), " with id ", std::to_string((unsigned) client), "\n");

                PRINT("vapaee::dex::client::action_add_client() ...\n");
            }
            
        };     
    };
};
