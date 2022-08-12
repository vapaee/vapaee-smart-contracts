#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/pay/modules/rex.hpp>
#include <vapaee/pay/modules/handler.hpp>
#include <vapaee/pay/modules/hub.hpp>
#include <vapaee/pay/modules/vip.hpp>
#include <vapaee/pay/modules/liquid.hpp>
#include <vapaee/pay/modules/billing.hpp>

namespace vapaee {

    CONTRACT vapaeepayhub : public eosio::contract {

        private:
            #include <vapaee/pay/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>

        public:
            using contract::contract;

            string get_version() { return string("1.1.0"); } // vapaeepayhub

            vapaeepayhub(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver;  vapaee::current_version = get_version();  }

            // Global module
            ACTION init() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::init() ------------------\n");
                vapaee::base::global::action_init();
            };

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
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::stakeconfig() ------------------\n");
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
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::stakepool() ------------------\n");
                vapaee::pay::rex::action_stakepool(action, token, poll_id, title, desc, locktime);
            }


            ACTION droponpool(
                asset quantity,
                name poll_id
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::droponpool() ------------------\n");
                vapaee::pay::rex::action_droponpool(quantity, poll_id);
            }

            // ---- actions for stakers ----

            ACTION stake(
                name owner,
                asset quantity,
                name poll_id
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::stake() ------------------\n");
                vapaee::pay::rex::action_stake(owner, quantity, poll_id);
            }

            ACTION unstake(
                name owner,
                asset quantity,
                name poll_id,
                std::vector<std::tuple<name, asset>> credits
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::unstake() ------------------\n");
                vapaee::pay::rex::action_unstake(owner, quantity, poll_id, credits);
            }

            ACTION takeprofits(
                name owner,
                symbol_code token,
                name poll_id
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::takeprofits() ------------------\n");
                vapaee::pay::rex::action_takeprofits(owner, token, poll_id);
            }            
          
            ACTION updtstake(
                name owner,
                symbol_code token,
                name poll_id
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updtstake() ------------------\n");
                vapaee::pay::rex::action_updtstake(owner, token, poll_id);
            }           
          
            ACTION mycredits(
                name owner,
                std::vector<std::tuple<name, asset>> credits
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::mycredits() ------------------\n");
                vapaee::pay::rex::action_mycredits(owner, credits);
            }


            // ---- actions for payhubs ----
            ACTION newpayhub(
                name admin,
                std::string vipname, 
                std::vector<std::tuple<asset,string>> recipients,
                std::vector<symbol_code> pockets,
                name billing_to                
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::newpayhub() ------------------\n");
                vapaee::pay::hub::action_newpayhub(admin, vipname, recipients, pockets, billing_to);
            }

            ACTION updatehub(
                name admin,
                uint64_t payhub_id, 
                std::vector<std::tuple<asset,string>> recipients,
                std::vector<symbol_code> pockets,
                name billing_to
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updatehub() ------------------\n");
                vapaee::pay::hub::action_updatehub(admin, payhub_id, recipients, pockets, billing_to);
            }

            ACTION newname(
                name owner,
                string vname
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::newname() ------------------\n");
                vapaee::pay::vip::action_newname(owner, vname);
            }

            ACTION movepocket(
                string target,
                name signer
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::movepocket() ------------------\n");
                vapaee::pay::hub::action_movepocket(target, signer);
            }

            // ---- actions for liquid pools
            ACTION newleakpool(
                name admin,
                uint64_t target,
                symbol_code token,
                string title,
                string desc,
                asset liquidity,
                asset issue_allaw,
                name easing,
                uint32_t epoch_start,
                uint32_t epoch_end
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::newleakpool() ------------------\n");
                vapaee::pay::liquid::action_newleakpool(admin, target, token, title, desc, liquidity, issue_allaw, easing, epoch_start, epoch_end);
            }

            ACTION udpleakpool(
                name admin,
                uint64_t leakpool_id,
                string title,
                string desc,
                asset issue_allaw_more
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::udpleakpool() ------------------\n");
                vapaee::pay::liquid::action_udpleakpool(admin, leakpool_id, title, desc, issue_allaw_more);
            }

            ACTION leakpool(uint64_t leakpool_id) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::leakpool() ------------------\n");
                vapaee::pay::liquid::action_leakpool(leakpool_id);
            }

            // ---- actions for billing
            ACTION billing(
                name admin,
                name invoice_name,
                symbol_code token,
                asset fixed,
                double percent,
                string payhub
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::billing() ------------------\n");
                vapaee::pay::billing::action_billing(admin, invoice_name, token, fixed, percent, payhub);
            }

            // ---- actions for billing
            ACTION invoice(
                asset quantity, asset fee, string memo
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::invoice() ------------------\n");
                vapaee::pay::billing::action_invoice(quantity, fee, memo);
            }


            [[eosio::on_notify("*::transfer")]]
            void htransfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER ",vapaee::current_contract.to_string(),"::htransfer() ------------------\n");

                // TODO
                vapaee::pay::handler::handle_pay_transfer(
                     from, to, quantity, memo, get_first_receiver());
            }

            AUX_DEBUG_CODE(
                
                TABLE tiempo_table {
                    uint64_t id;
                    time_point_sec tiempo;
                    uint32_t numero;
                    uint64_t primary_key() const { return id; }
                };
                typedef eosio::multi_index< "tiempo"_n, tiempo_table> tiempo;

                ACTION settiempo(int nonce) {
                    PRINT("\nACTION ",vapaee::current_contract.to_string(),"::settiempo() ------------------\n");
                    
                    tiempo ttable(get_self(), get_self().value);
                    auto ptr = ttable.find(0);
                    if (ptr == ttable.end()) {
                        ttable.emplace(get_self(), [&](auto &a){
                            a.id     = 0;
                            a.tiempo = time_point_sec(current_time_point());
                            a.numero = a.tiempo.sec_since_epoch();
                        });
                    } else {
                        ttable.modify(*ptr, get_self(), [&](auto &a){
                            a.tiempo = time_point_sec(current_time_point());
                            a.numero = a.tiempo.sec_since_epoch();
                        });
                    }
                }

                // time_point_sec now = time_point_sec(current_time_point());
            )

            
    };  // contract class

};  // namespace vapaee
