#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/dispatcher.spp>
#include <vapaee/dex/modules/client.hpp>
#include <vapaee/dex/modules/token.hpp>
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
                bool tradeable
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.addtoken() ------------------\n");
                vector<uint64_t> groups = {0};
                vapaee::dex::token::action_add_token(contract, symbol, precision, admin);
                vapaee::dex::token::action_update_token_info(symbol, title, website, brief, banner, icon, iconlg, pcontact, gcontact, groups, tradeable);
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
                vector<uint64_t> groups,
                bool tradeable
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.updatetoken() ------------------\n");
                vapaee::dex::token::action_update_token_info(sym_code, title, website, brief, banner, icon, iconlg, pcontact, gcontact, groups, tradeable);
            };

            ACTION tokenadmin (
                const symbol_code & sym_code,
                name admin
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.tokenadmin() ------------------\n");
                vapaee::dex::token::action_set_token_admin(sym_code, admin);
            };

            ACTION setcurrency (
                const symbol_code & sym_code,
                bool is_currency,
                uint64_t token_group
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.setcurrency() ------------------\n");
                vapaee::dex::token::action_set_token_as_currency(sym_code, is_currency, token_group);
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

            ACTION addtnkgroup (
                name admin,
                string title,
                string website,
                string brief,
                string banner,
                string thumbnail) {
                MAINTENANCE(); 
                PRINT("\nACTION telosmaindex.addtnkgroup() ------------------\n");
                vapaee::dex::token::action_add_token_group(admin, title, website, brief, banner, thumbnail);
            };
            
            ACTION uptnkgroup (
                uint64_t group_id,
                name admin,
                string title,
                string website,
                string brief,
                string banner,
                string thumbnail
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.uptnkgroup() ------------------\n");
                vapaee::dex::token::action_update_token_group(group_id, admin, title, website, brief, banner, thumbnail);
            };        

            ACTION chnggroups (
                const symbol_code & sym_code,
                vector<uint64_t> groups
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.chnggroups() ------------------\n");
                vapaee::dex::token::action_change_groups_for_a_token(sym_code, groups);
            };

            ACTION newmarket(
                const symbol_code & commodity,
                const symbol_code & currency
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosmaindex.newmarket() ------------------\n");
                vapaee::dex::market::action_newmarket(commodity, currency);
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
                name sender,
                name recipient,
                asset rate,
                asset sent,
                asset result
            ) {
                // TODO: this does not allow other contracts to participate in the protocol
                require_auth(vapaee::pool::contract);
                vapaee::dex::record::action_record_pool_swap(
                    sender, recipient, rate, sent, result);
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
                // TODO: this does not allow other contracts to participate in the protocol
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
                vapaee::dex::token::init();
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
                
                if (memo == "dao") {
                    MAINTENANCE();
                    vapaee::dex::dao::handle_dex_transfer(
                        from, to, quantity, memo, get_first_receiver());
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
