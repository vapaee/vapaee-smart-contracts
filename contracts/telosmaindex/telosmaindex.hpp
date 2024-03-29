#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/token/modules/utils.hpp>
#include <vapaee/dex/modules/client.hpp>
#include <vapaee/dex/modules/token.hpp>
#include <vapaee/dex/modules/fees.hpp>
#include <vapaee/dex/modules/swap.hpp>
#include <vapaee/dex/modules/dao.hpp>
#include <vapaee/dex/modules/maintenance.hpp>
#include <vapaee/dex/modules/experience.hpp>
#include <vapaee/book/modules/deposit.hpp>

namespace vapaee {
    using namespace dex;

    CONTRACT telosmaindex : public eosio::contract {
        
        private:

            #include <vapaee/dex/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>            

        public:
            using contract::contract;

            string get_version() { return string("0.9.6"); } // telosmaindex-0.9.6 - recalculate the inverse using extended symbol in regpoolswap

            telosmaindex(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver;  vapaee::current_version = get_version();  }

            // Global module
            ACTION init() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::init() ------------------\n");
                vapaee::base::global::action_init();
                vapaee::dex::global::init();
            };

            // Client Module
            ACTION addclient (
                name admin,
                name receiver,
                string params,
                string title,
                string website,
                string brief,
                string banner,
                string thumbnail
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::addclient() ------------------\n");
                vapaee::dex::client::action_add_client(admin, receiver, params, title, website, brief, banner, thumbnail);
            };
            
            ACTION updateclient (
                uint64_t client,
                name admin,
                name receiver,
                string params,
                string title,
                string website,
                string brief,
                string banner,
                string thumbnail
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updateclient() ------------------\n");
                vapaee::dex::client::action_update_client(client, admin, receiver, params, title, website, brief, banner, thumbnail);
            };

            // Token Module
            ACTION addtoken (
                name contract,
                const symbol_code & symbol,
                uint8_t precision,
                name admin,
                string title,
                string website,
                string brief,
                string banner,
                string icon, string iconlg,
                string pcontact,
                string gcontact,
                bool tradeable,
                bool stable
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::addtoken() ------------------\n");
                vapaee::dex::token::action_add_token(contract, symbol, precision, admin);
                vapaee::dex::token::action_update_token_info(symbol, title, website, brief, banner, icon, iconlg, pcontact, gcontact, tradeable, stable);
            };
            
            ACTION updatetoken (
                const symbol_code & sym_code,
                string title,
                string website,
                string brief,
                string banner,
                string icon,
                string iconlg,
                string pcontact,
                string gcontact,
                bool tradeable,
                bool stable
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updatetoken() ------------------\n");
                vapaee::dex::token::action_update_token_info(sym_code, title, website, brief, banner, icon, iconlg, pcontact, gcontact, tradeable, stable);
            };

            ACTION tokenadmin (
                const symbol_code & sym_code,
                name admin
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::tokenadmin() ------------------\n");
                vapaee::dex::token::action_set_token_admin(sym_code, admin);
            };

            ACTION settokendata (
                const symbol_code & sym_code,
                uint64_t id,
                name action,
                string text,
                string link,
                name shownas
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updatetoken() ------------------\n");
                vapaee::dex::token::action_set_token_data(sym_code, id, action, text, link, shownas);
            };

            ACTION addcurrency (
                const symbol_code & sym_code,
                string website,
                string brief
            ) {
                MAINTENANCE(); 
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::addcurrency() ------------------\n");
                vapaee::dex::token::action_add_currency(sym_code, website, brief);
            };
            
            ACTION udpcurrency (
                uint64_t currency_id,
                string website,
                string brief
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::udpcurrency() ------------------\n");
                vapaee::dex::token::action_update_currency_data(currency_id, website, brief);
            };

            ACTION newmarket(
                const symbol_code & commodity,
                const symbol_code & currency,
                name converter
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::newmarket() ------------------\n");
                vapaee::dex::market::action_newmarket(commodity, currency, converter);
            };

            // record event entry
            // ACTION event (
            //     name user,
            //     name event,
            //     string params,
            //     time_point_sec date
            // ) {
            //     MAINTENANCE();
            //     PRINT("\nACTION ",vapaee::current_contract.to_string(),"::event() ------------------\n");
            //     check(vapaee::dex::record::aux_check_allowed_to_record_entry(), ERROR_EVT_1);
            // };

            // record history entry
            ACTION history (
                name buyer,
                name seller,
                bool isbuy,
                asset price,
                asset inverse,
                asset amount,
                asset payment,
                asset buyfee,
                asset sellfee,
                time_point_sec date
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::history() ------------------\n");
                require_recipient( name("teloshistory") );
                check(vapaee::dex::record::aux_check_allowed_to_record_entry(), ERROR_HIST_1);
            };

            // record history hour block
            // ACTION historyblock (
            //     asset price,    // current price for this hour (and last)
            //     asset inverse,
            //     asset entrance, // first price for this hour
            //     asset max,      // max price for this hour
            //     asset min,      // min price for this hour
            //     asset volume,
            //     asset amount,
            //     uint64_t hour,
            //     time_point_sec date
            // ) {
            //     MAINTENANCE();
            //     PRINT("\nACTION ",vapaee::current_contract.to_string(),"::historyblock() ------------------\n");
            //     require_recipient( name("teloshistory") );
            //     check(vapaee::dex::record::aux_check_allowed_to_record_entry(), ERROR_HISTBLK_1);
            // };


            // Experience Module
            ACTION reward (
                name user,
                const asset & points,
                const asset & exp
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::reward() ------------------\n");
                vapaee::dex::experience::action_reward_user(user, points, exp);
            };

            // Pool dex record swap
            ACTION regpoolswap(
                name recipient,
                name converter,
                asset rate,
                asset sent,
                asset result,
                asset fee
            ) {
                require_auth(converter);
                // check(get_first_receiver() == converter, create_error_name2(ERROR_RPS_1, converter, get_first_receiver()));
                vapaee::dex::record::action_record_pool_swap(
                    recipient, converter, rate, sent, result, fee);
            };

            // Update swap converter state
            ACTION updpoolswap(
                uint64_t market,
                name converter
            ) {
                if (has_auth(converter)) {
                    require_auth(converter);
                } else {
                    require_auth(vapaee::dex::contract);
                }
                vapaee::dex::record::action_update_pool_swap_state(market, converter);
            };

            // Book dex record deal
            ACTION regbookdeal(
                name type,
                name buyer,
                name seller,
                asset price,   // unit price
                asset inverse, // inverse of unit price in commodity sym
                asset payment, // units of commodity
                asset amount,  // total price
                asset buyfee, 
                asset sellfee
            ) {
                require_auth(vapaee::book::contract);
                vapaee::dex::record::action_record_book_deal(
                    type, buyer, seller, price, inverse, payment, amount, buyfee, sellfee);
            };

            // Maintenance Module
            ACTION maintenance (name credits_to) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::maintenance() ------------------\n");
                vapaee::dex::maintenance::action_do_maintenance(credits_to);
            };
            
            // DAO Module
            ACTION balloton (
                name operation,
                vector<string> params,
                string arguments,
                name feepayer
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::balloton() ------------------\n");
                vapaee::dex::dao::action_start_ballot_on(operation, params, arguments, feepayer);
            };

            [[eosio::on_notify("telos.decide::broadcast")]]
            void hbroadcast(
                name ballot_name,
                map<name, asset> endresults,
                uint32_t total_voters
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER telosmaindex::hbroadcast() ------------------\n");
                vapaee::dex::dao::handler_ballot_result(ballot_name, endresults, total_voters);
            };

            [[eosio::on_notify("*::transfer")]]
            void htransfer(
                name from,
                name to,
                asset quantity,
                string  memo
            ) {
                // MAINTENANCE();
                PRINT("\nHANDLER telosbookdex.htransfer() ------------------\n");
                
                if (vapaee::base::global::
                    handler_should_ignore_transfer(from, to, quantity, memo, get_first_receiver())
                ) return;
                
                MAINTENANCE();

                if (memo == "ballot" || memo == "addtoken") {
                    // user is paying fees for ballot or adding a token
                    vapaee::dex::fees::handle_dex_transfer(
                        from, to, quantity, memo, get_first_receiver()
                    );
                } else {
                    // user is swapping tokens
                    vapaee::dex::swap::handle_start_swap_transfer(
                        from, to, quantity, memo, get_first_receiver()
                    );                    
                }
                
            }

            ACTION updatenow() {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updatenow() ------------------\n");
                vapaee::dex::global::action_updatenow();
            }
                    
            AUX_DEBUG_CODE (

                ACTION testdao (name ballotname, map<name, asset> finalresults, uint32_t totalvoters) {
                    MAINTENANCE();
                    PRINT("\nACTION ",vapaee::current_contract.to_string(),"::testdao() ------------------\n");
                    hbroadcast(ballotname, finalresults, totalvoters);
                };

                ACTION timeoffset (uint32_t offset) {
                    PRINT("\nACTION ",vapaee::current_contract.to_string(),"::timeoffset() ------------------\n");
                    vapaee::dex::global::action_set_time_offset(offset);
                };

            )

            ACTION hotfix() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::hotfix() ------------------\n");
                
                require_auth(vapaee::dex::contract);

                // cambiaremos la propiedad a.website = string("https://vapaee.com/dex"); del id=0
                clients clients_table(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = clients_table.find(0);
                check(itr != clients_table.end(), "ERROR_HF_1");
                clients_table.modify(itr, vapaee::dex::contract, [&](auto &row) {
                    row.website = string("https://vapaee.com/dex");
                });


                // cambiaremos la propiedad a.converter = name("koinonospool"); del id=3
                converters converters_table(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr2 = converters_table.find(3);
                check(itr2 != converters_table.end(), "ERROR_HF_2");
                converters_table.modify(itr2, vapaee::dex::contract, [&](auto &row) {
                    row.converter = name("koinonospool");
                });
            }
    }; // contract class

}; // vapaee namespace
