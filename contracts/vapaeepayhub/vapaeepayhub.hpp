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

            string get_version() { return string("1.2.8"); } // vapaeepayhub-1.2.8

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
                // vapaee::pay::liquid::action_update(helper);
                vapaee::pay::hub::action_update(helper);
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
                name signer,
                string memo
            ) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::movepocket() ------------------\n");
                vapaee::pay::hub::action_movepocket(target, signer, memo);
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

            ACTION clear() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::clear() ------------------\n");
                require_auth(get_self()); // Only the contract itself can call this

                // --- Leakpools ---
                // Scope: get_self()
                PRINT("Clearing leakpools table...\n");
                leakpools leakpools_tbl(get_self(), get_self().value);
                auto lp_itr = leakpools_tbl.begin();
                while (lp_itr != leakpools_tbl.end()) {
                    lp_itr = leakpools_tbl.erase(lp_itr);
                }

                // --- Stakeconfigs and collect token symbols ---
                // Scope: get_self()
                PRINT("Clearing stakeconfigs table and collecting token symbols...\n");
                std::vector<symbol_code> token_symbols_to_clear;
                stakeconfigs stakeconfigs_tbl(get_self(), get_self().value);
                auto sc_itr = stakeconfigs_tbl.begin();
                while (sc_itr != stakeconfigs_tbl.end()) {
                    token_symbols_to_clear.push_back(sc_itr->token);
                    sc_itr = stakeconfigs_tbl.erase(sc_itr);
                }

                // --- Loop through collected token symbols ---
                for (const auto& token_sym_code : token_symbols_to_clear) {
                    PRINT("Processing token: ", token_sym_code.to_string(), "\n");

                    // - Stakepools
                    // Scope: token_sym_code.raw()
                    PRINT("  Clearing stakepools for token: ", token_sym_code.to_string(), "\n");
                    stakepools stakepools_tbl(get_self(), token_sym_code.raw());
                    auto sp_itr = stakepools_tbl.begin();
                    while (sp_itr != stakepools_tbl.end()) {
                        sp_itr = stakepools_tbl.erase(sp_itr);
                    }

                    // - Invoices
                    // Scope: token_sym_code.raw()
                    PRINT("  Clearing invoices for token: ", token_sym_code.to_string(), "\n");
                    invoices invoices_tbl(get_self(), token_sym_code.raw());
                    auto inv_itr = invoices_tbl.begin();
                    while (inv_itr != invoices_tbl.end()) {
                        inv_itr = invoices_tbl.erase(inv_itr);
                    }

                    // - Stakers and their dependent tables
                    // Scope: token_sym_code.raw()
                    PRINT("  Clearing stakers and their dependent tables for token: ", token_sym_code.to_string(), "\n");
                    stakers stakers_tbl(get_self(), token_sym_code.raw());
                    auto stk_itr = stakers_tbl.begin();
                    while (stk_itr != stakers_tbl.end()) {
                        name staker_account = stk_itr->account;
                        PRINT("    Processing staker: ", staker_account.to_string(), "\n");

                        // -- Mystake
                        // Scope: staker_account.value, PK: token_sym_code.raw()
                        PRINT("      Clearing mystake for staker: ", staker_account.to_string(), " and token: ", token_sym_code.to_string(), "\n");
                        mystake mystake_tbl(get_self(), staker_account.value);
                        auto ms_itr = mystake_tbl.find(token_sym_code.raw()); // mystake's PK is the token symbol_code
                        if (ms_itr != mystake_tbl.end()) {
                            mystake_tbl.erase(ms_itr);
                        }

                        // -- Mypoolstake
                        // Scope: staker_account.value
                        // We need to iterate and check the token symbol for each entry, as mypoolstake is not directly scoped by token for its main table.
                        PRINT("      Clearing mypoolstake for staker: ", staker_account.to_string(), " and token: ", token_sym_code.to_string(), "\n");
                        mypoolstake mypoolstake_tbl(get_self(), staker_account.value);
                        auto mps_itr = mypoolstake_tbl.begin();
                        while (mps_itr != mypoolstake_tbl.end()) {
                            if (mps_itr->stake.symbol.code() == token_sym_code) { // Assuming 'stake' asset holds the relevant token
                                mps_itr = mypoolstake_tbl.erase(mps_itr);
                            } else {
                                ++mps_itr;
                            }
                        }

                        // Erase the staker entry itself for this token scope
                        stk_itr = stakers_tbl.erase(stk_itr);
                    }
                }

                // --- Names ---
                // Scope: get_self()
                PRINT("Clearing names table...\n");
                names names_tbl(get_self(), get_self().value);
                auto n_itr = names_tbl.begin();
                while (n_itr != names_tbl.end()) {
                    n_itr = names_tbl.erase(n_itr);
                }

                // --- Payhubs ---
                // Scope: get_self()
                PRINT("Clearing payhubs table...\n");
                payhubs payhubs_tbl(get_self(), get_self().value);
                auto ph_itr = payhubs_tbl.begin();
                while (ph_itr != payhubs_tbl.end()) {
                    ph_itr = payhubs_tbl.erase(ph_itr);
                }

                // --- Payments ---
                // Scope: get_self()
                PRINT("Clearing payments table...\n");
                payments payments_tbl(get_self(), get_self().value);
                auto p_itr = payments_tbl.begin();
                while (p_itr != payments_tbl.end()) {
                    p_itr = payments_tbl.erase(p_itr);
                }

                // --- Paypockets ---
                // Scope: get_self()
                PRINT("Clearing paypockets table...\n");
                paypockets paypockets_tbl(get_self(), get_self().value);
                auto pp_itr = paypockets_tbl.begin();
                while (pp_itr != paypockets_tbl.end()) {
                    pp_itr = paypockets_tbl.erase(pp_itr);
                }

                PRINT("All specified tables cleared.\n");
            } // ACTION clear()




            
    };  // contract class

};  // namespace vapaee
