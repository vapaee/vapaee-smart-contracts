#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/token/modules/utils.hpp>
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

            string get_version() { return string("1.2.5"); } // vapaeepayhub-1.2.5 - ACTION update(helper) added to leak and rotate leakpools

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
                std::vector<name> categories,
                string dayslock
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::stakeconfig() ------------------\n");
                vapaee::pay::rex::action_stakeconfig(action, admin, token, contract, title, categories, dayslock);
            }

            ACTION stakepool(
                name action,
                symbol_code token,
                name poollabel,
                string title,
                string locktime
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::stakepool() ------------------\n");
                vapaee::pay::rex::action_stakepool(action, token, poollabel, title, locktime);
            }


            ACTION droponpool(
                asset quantity,
                name poollabel
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::droponpool() ------------------\n");
                vapaee::pay::rex::action_droponpool(quantity, poollabel);
            }

            // ---- actions for stakers ----

            ACTION stake(
                name owner,
                asset quantity,
                name poollabel
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::stake() ------------------\n");
                vapaee::pay::rex::action_stake(owner, quantity, poollabel);
            }

            ACTION unstake(
                name owner,
                asset quantity,
                name poollabel,
                std::vector<std::tuple<name, asset>> credits
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::unstake() ------------------\n");
                vapaee::pay::rex::action_unstake(owner, quantity, poollabel, credits);
            }

            ACTION takeprofits(
                name owner,
                symbol_code token,
                name poollabel
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::takeprofits() ------------------\n");
                vapaee::pay::rex::action_takeprofits(owner, token, poollabel);
            }            
          
            ACTION updtstake(
                name owner,
                symbol_code token,
                name poollabel
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updtstake() ------------------\n");
                vapaee::pay::rex::action_updtstake(owner, token, poollabel);
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
                name billingto                
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::newpayhub() ------------------\n");
                vapaee::pay::hub::action_newpayhub(admin, vipname, recipients, pockets, billingto);
            }

            ACTION update(
                name helper
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::update() ------------------\n");
                vapaee::pay::liquid::action_update(helper);
            }

            ACTION updatehub(
                name admin,
                uint64_t payhub_id, 
                std::vector<std::tuple<asset,string>> recipients,
                std::vector<symbol_code> pockets,
                name billingto
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::updatehub() ------------------\n");
                vapaee::pay::hub::action_updatehub(admin, payhub_id, recipients, pockets, billingto);
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

            // ---- delayed paymets
            ACTION pay(
                asset quantity,
                string target,
                string memo,
                bool move
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::pay() ------------------\n");
                vapaee::pay::hub::action_schedule_pay(quantity, target, memo, move);
            }

            ACTION movepayment(
                string target,
                symbol_code token,
                name signer
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::movepayment() ------------------\n");
                vapaee::pay::hub::action_movepayment(target, token, signer);
            }

            // ---- actions for liquid pools
            ACTION newleakpool(
                name admin,
                uint64_t target,
                symbol_code token,
                string title,
                asset liquidity,
                asset issueallaw,
                name easing,
                uint32_t epochstart,
                uint32_t epochend
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::newleakpool() ------------------\n");
                vapaee::pay::liquid::action_newleakpool(admin, target, token, title, liquidity, issueallaw, easing, epochstart, epochend);
            }

            ACTION udpleakpool(
                name admin,
                uint64_t leakpoolid,
                string title,
                asset issuemore
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::udpleakpool() ------------------\n");
                vapaee::pay::liquid::action_udpleakpool(admin, leakpoolid, title, issuemore);
            }

            ACTION leakpool(uint64_t leakpoolid, uint64_t random) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::leakpool() ------------------\n");
                vapaee::pay::liquid::action_leakpool(leakpoolid);
            }

            // ---- actions for billing
            ACTION billing(
                name admin,
                name invname,
                symbol_code token,
                asset fixed,
                double percent,
                string payhub
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::billing() ------------------\n");
                vapaee::pay::billing::action_billing(admin, invname, token, fixed, percent, payhub);
            }

            // ---- actions for billing
            ACTION invoice(
                const name& payer,
                const name& seller,
                const name& collector,
                const asset& quantity,
                const asset& fee,
                const asset& fiat,
                const string& memo
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::invoice() ------------------\n");
                vapaee::pay::billing::action_invoice(payer, seller, collector, quantity, fee, fiat, memo);
            }

            // ---- Staking Mature Slots
            ACTION resetmslot(name owner, symbol_code token, name poollabel, int slotindex) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::resetmslot() ------------------\n");
                vapaee::pay::rex::action_resetmslot(owner, token, poollabel, slotindex);
            }

            ACTION buymslot(name owner, symbol_code token, name poollabel, int slots) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::resetmslot() ------------------\n");
                vapaee::pay::rex::action_buymslot(owner, token, poollabel, slots);
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

            // AUX_DEBUG_CODE(
                
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
            // )

            
    };  // contract class

};  // namespace vapaee
