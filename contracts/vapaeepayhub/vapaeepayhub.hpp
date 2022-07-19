#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/pay/modules/rex.hpp>
#include <vapaee/pay/modules/handler.hpp>
#include <vapaee/pay/modules/hub.hpp>
#include <vapaee/pay/modules/vip.hpp>

namespace vapaee {

    CONTRACT vapaeepayhub : public eosio::contract {

        private:
            #include <vapaee/pay/tables.all.hpp>

        public:
            using contract::contract;


            vapaeepayhub(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }

            // ---- actions for token admin ----

            ACTION stakeconfig(
                name action,
                name admin,
                symbol_code token,
                name contract,
                string title,
                string desc,
                std::vector<name> categories,
                string credits_locktime
            ) {
                vapaee::pay::rex::action_stakeconfig(action, admin, token, contract, title, desc, categories, credits_locktime);
            }

            ACTION stakepool(
                name action,
                symbol_code token,
                name poll_id,
                string title,
                string desc,
                string locktime
            ) {
                vapaee::pay::rex::action_stakepool(action, token, poll_id, title, desc, locktime);
            }


            ACTION droponpool(
                asset quantity,
                name poll_id
            ) {
                vapaee::pay::rex::action_droponpool(quantity, poll_id);
            }

            // ---- actions for stakers ----

            ACTION stake(
                name owner,
                asset quantity,
                name poll_id
            ) {
                vapaee::pay::rex::action_stake(owner, quantity, poll_id);
            }

            ACTION unstake(
                name owner,
                asset quantity,
                name poll_id,
                std::vector<std::tuple<name, asset>> credits
            ) {
                vapaee::pay::rex::action_unstake(owner, quantity, poll_id, credits);
            }

            ACTION takeprofits(
                name owner,
                symbol_code token,
                name poll_id
            ) {
                vapaee::pay::rex::action_takeprofits(owner, token, poll_id);
            }            
          
            ACTION updtstake(
                name owner,
                symbol_code token,
                name poll_id
            ) {
                vapaee::pay::rex::action_updtstake(owner, token, poll_id);
            }           
          
            ACTION mycredits(
                name owner,
                std::vector<std::tuple<name, asset>> credits
            ) {
                vapaee::pay::rex::action_mycredits(owner, credits);
            }


            // ---- actions for payhubs ----
            
            ACTION newpayhub(
                name admin,
                std::string vipname, 
                std::vector<symbol_code> tokens,
                std::vector<std::tuple<asset,string>> recipients
            ) {
                vapaee::pay::hub::action_newpayhub(admin, vipname, tokens, recipients);
            }

            ACTION updatehub(
                name admin,
                uint64_t payhub_id, 
                std::vector<symbol_code> tokens,
                std::vector<std::tuple<asset,string>> recipients
            ) {
                vapaee::pay::hub::action_updatehub(admin, payhub_id, tokens, recipients);
            }

            ACTION newname(
                name owner,
                string vname
            ) {
                vapaee::pay::vip::action_newname(owner, vname);
            }

            ACTION movepocket(
                string target,
                name signer
            ) {
                vapaee::pay::hub::action_movepocket(target, signer);
            }

            [[eosio::on_notify("*::transfer")]]
            void htransfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER vapaeepayhub.htransfer() ------------------\n");


                // TODO
                vapaee::pay::handler::handle_pay_transfer(
                     from, to, quantity, memo, get_first_receiver());
            }
            
    };  // contract class

};  // namespace vapaee
