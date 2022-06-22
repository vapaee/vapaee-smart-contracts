#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/dispatcher.spp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/modules/client.hpp>
#include <vapaee/dex/modules/token.hpp>
#include <vapaee/dex/modules/fees.hpp>
#include <vapaee/dex/modules/swap.hpp>
#include <vapaee/dex/modules/dao.hpp>
#include <vapaee/dex/modules/maintenance.hpp>
#include <vapaee/dex/modules/experience.hpp>
#include <vapaee/book/modules/deposit.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace dex;

    CONTRACT telosmaindex : public eosio::contract {
        
        private:

            #include <vapaee/dex/tables.all.hpp>

        public:
            using contract::contract;

            telosmaindex(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }

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
                PRINT("\nACTION telosmaindex.addclient() ------------------\n");
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
                PRINT("\nACTION telosmaindex.updateclient() ------------------\n");
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
                PRINT("\nACTION telosmaindex.addtoken() ------------------\n");
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
                PRINT("\nACTION telosmaindex.updatetoken() ------------------\n");
                vapaee::dex::token::action_update_token_info(sym_code, title, website, brief, banner, icon, iconlg, pcontact, gcontact, tradeable, stable);
            };

            ACTION tokenadmin (
                const symbol_code & sym_code,
                name admin
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.tokenadmin() ------------------\n");
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
                PRINT("\nACTION telosmaindex.updatetoken() ------------------\n");
                vapaee::dex::token::action_set_token_data(sym_code, id, action, text, link, shownas);
            };

            ACTION addcurrency (
                const symbol_code & sym_code,
                string website,
                string brief) {
                MAINTENANCE(); 
                PRINT("\nACTION telosmaindex.addcurrency() ------------------\n");
                vapaee::dex::token::action_add_currency(sym_code, website, brief);
            };
            
            ACTION udpcurrency (
                uint64_t currency_id,
                string website,
                string brief
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.udpcurrency() ------------------\n");
                vapaee::dex::token::action_update_currency_data(currency_id, website, brief);
            };

            ACTION newmarket(
                const symbol_code & commodity,
                const symbol_code & currency,
                name converter
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.newmarket() ------------------\n");
                vapaee::dex::market::action_newmarket(commodity, currency, converter);
            };

            // record event entry
            ACTION event (
                name user,
                name event,
                string params,
                time_point_sec date
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.event() ------------------\n");
                check(vapaee::dex::record::aux_check_allowed_to_record_entry(), ERROR_EVT_1);
            };

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
                PRINT("\nACTION telosmaindex.history() ------------------\n");
                check(vapaee::dex::record::aux_check_allowed_to_record_entry(), ERROR_HIST_1);
            };

            // record history hour block
            ACTION historyblock (
                asset price,    // current price for this hour (and last)
                asset inverse,
                asset entrance, // first price for this hour
                asset max,      // max price for this hour
                asset min,      // min price for this hour
                asset volume,
                asset amount,
                uint64_t hour,
                time_point_sec date
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.historyblock() ------------------\n");
                check(vapaee::dex::record::aux_check_allowed_to_record_entry(), ERROR_HISTBLK_1);
            };


            // Experience Module
            ACTION reward (
                name user,
                const asset & points,
                const asset & exp
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.reward() ------------------\n");
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
                PRINT("\nACTION telosmaindex.maintenance() ------------------\n");
                vapaee::dex::maintenance::action_do_maintenance(credits_to);
            };

            // Global Module
            ACTION init () {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.init() ------------------\n");
                vapaee::dex::global::init();
            };

            
            // DAO Module
            ACTION balloton (
                name operation,
                vector<string> params,
                string arguments,
                name feepayer
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.balloton() ------------------\n");
                vapaee::dex::dao::action_start_ballot_on(operation, params, arguments, feepayer);
            };

            // handler for telos.decide::broadcast
            HANDLER hbroadcast(
                name ballot_name,
                map<name, asset> final_results,
                uint32_t total_voters
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER telosmaindex.hbroadcast() ------------------\n");
                vapaee::dex::dao::handler_ballot_result(ballot_name, final_results, total_voters);
            };

            // handler for *::transfer
            HANDLER htransfer(
                name from,
                name to,
                asset quantity,
                string  memo
            ) {
                // MAINTENANCE();
                PRINT("\nHANDLER telosbookdex.htransfer() ------------------\n");
                
                // skip handling transfers from this contract to outside
                if (from == vapaee::dex::contract)
                    return;

                // skip handling transfers to other contracts then this one
                if (to != vapaee::dex::contract) {
                    return;
                }
                
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
                    
            AUX_DEBUG_CODE (

                ACTION testdao (name ballot_name, map<name, asset> final_results, uint32_t total_voters) {
                    MAINTENANCE();
                    PRINT("\nACTION telosmaindex.testdao() ------------------\n");
                    hbroadcast(ballot_name, final_results, total_voters);
                };

                ACTION timeoffset (uint32_t offset) {
                    PRINT("\nACTION telosmaindex.timeoffset() ------------------\n");
                    vapaee::dex::global::action_set_time_offset(offset);
                };

            )
    }; // contract class

}; // vapaee namespace
