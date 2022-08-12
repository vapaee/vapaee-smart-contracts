#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/dao.hpp>
#include <vapaee/dex/modules/fees.hpp>

using namespace std;
#define TOKEN_GROUP_ZERO 0

namespace vapaee {
    namespace dex {
        using namespace utils;
        
        namespace token {

            asset get_token_supply(const symbol_code& token) {
                PRINT("vapaee::dex::token::get_token_supply(",token.to_string(),")\n");
                // vapaee::dex::token::get_token_supply(token)

PRINT("CHECKPOINT A\n");

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(token.raw());

                if (itr != tokenstable.end()) {
                    stats stat_table( itr->contract, token.raw() );
                    auto ptr = stat_table.find(token.raw());
                    if (ptr != stat_table.end()) {

                        PRINT(" > ptr->supply!! ", ptr->supply.to_string(),"\n");

                        return ptr->supply;
                    }                    
                }
                
                // else
                PRINT(" < not found!! \n");

                AUX_DEBUG_CODE(
                    // this debug code is only to show what table contains
                    for (
                        auto ptr = tokenstable.begin();
                        ptr != tokenstable.end();
                        ++ptr
                    ) {
                        PRINT(" symbol: ", ptr->symbol.to_string());
                        PRINT(" contract: ", ptr->contract.to_string());
                        stats stat_table( itr->contract, token.raw() );
                        auto aux = stat_table.find(token.raw());
                        if (aux != stat_table.end()) {
                            PRINT(" supply: ", aux->supply.to_string());
                        }
                        PRINT("\n");                    
                    }
                )                

                return asset(0,symbol(token,0));
            }

            name get_token_contract(const symbol_code& token) {
                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(token.raw());

                if (itr != tokenstable.end()) {
                    return itr->contract;
                } else {
                    return name();
                }
            }

            name get_asset_token_contract(asset quantity) {
                return get_token_contract(quantity.symbol.code());
            }

            // tokens ---------------- 
            
            void action_add_token(name tcontract, const symbol_code & sym_code, uint8_t precision, name admin) {
                PRINT("vapaee::dex::token::action_add_token()\n");
                PRINT(" tcontract: ", tcontract.to_string(), "\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                PRINT(" precision: ", std::to_string((unsigned) precision), "\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                
                // check if tokens existe in token contract account name
                stats statstable(tcontract, sym_code.raw());
                auto token_itr = statstable.find( sym_code.raw() );
                check(token_itr != statstable.end(), create_error_symcode1(ERROR_AAT_1, sym_code).c_str());

                check(
                    has_auth(vapaee::dex::contract) || has_auth(tcontract) || has_auth(token_itr->issuer),
                    "only token contract or issuer can add this token to DEX"
                );

                name ram_payer = has_auth(vapaee::dex::contract) ? vapaee::dex::contract : (has_auth(tcontract) ? tcontract : token_itr->issuer);


                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr == tokenstable.end(), create_error_symcode1(ERROR_AAT_2, sym_code).c_str());
                tokenstable.emplace( ram_payer, [&]( auto& a ){
                    a.contract  = tcontract;
                    a.symbol    = sym_code;
                    a.precision = precision;
                    a.admin     = admin;
                    a.title     = "";
                    a.website   = "";
                    a.brief     = "";
                    a.banner    = "";
                    a.icon      = "";
                    a.iconlg    = "";
                    a.tradeable = false;
                    a.date      = vapaee::dex::global::get_now_time_point_sec();
                    a.data      = 0;
                    a.currency  = 0;
                    a.stable    = false;
                });
                PRINT(" -> tokenstable.emplace() OK\n");

                if (admin != vapaee::dex::contract) {
                    // This is the normal case in which the token is added by third party user

                    // charge feepayer for the fees of registering a token
                    vapaee::dex::fees::aux_delete_fees(vapaee::dex::fees::concept_addtoken, admin);

                    // TODO: hadle profits
                }

                vapaee::dex::record::aux_register_event(admin, name("new.token"), string("token|") + sym_code.to_string() + " (" + vapaee::dex::contract.to_string() + ")");

                PRINT("vapaee::dex::token::action_add_token() ...\n");
            }

            void action_update_token_info(
                const symbol_code & sym_code,
                string title, string website,
                string brief, string banner,
                string icon,
                string iconlg,
                string pcontact,
                string gcontact,
                bool tradeable,
                bool stable
            ) {
                PRINT("vapaee::dex::token::action_update_token_info()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" website: ", website.c_str(), "\n");
                PRINT(" brief: ", brief.c_str(), "\n");
                PRINT(" banner: ", banner.c_str(), "\n");
                PRINT(" icon: ", icon.c_str(), "\n");
                PRINT(" iconlg: ", iconlg.c_str(), "\n");
                PRINT(" pcontact: ", pcontact.c_str(), "\n");
                PRINT(" gcontact: ", gcontact.c_str(), "\n");
                PRINT(" tradeable: ", std::to_string(tradeable), "\n");
                PRINT(" stable: ", std::to_string(stable), "\n");

                tokens tokenstable(contract, contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_AUTI_1, sym_code).c_str());
                name admin = itr->admin;
                check(has_auth(contract) || has_auth(admin), ERROR_AUTI_2);

                // is it blacklisted?
                check(!vapaee::dex::security::aux_is_token_blacklisted(itr->symbol, itr->contract), 
                    create_error_symcode1(ERROR_AUTI_3, itr->symbol).c_str());
                
                tokenstable.modify( *itr, same_payer, [&]( auto& a ){
                    a.title      = title;
                    a.website    = website;
                    a.brief      = brief;
                    a.banner     = banner;
                    a.icon       = icon;
                    a.iconlg     = iconlg;
                    a.pcontact   = pcontact;
                    a.gcontact   = gcontact;
                    a.tradeable  = tradeable;
                    a.date       = vapaee::dex::global::get_now_time_point_sec();
                    a.stable     = stable;
                });

                PRINT("vapaee::dex::token::action_update_token_info() ...\n");
            }

            void action_set_token_admin (const symbol_code & sym_code, name newadmin) {
                PRINT("vapaee::dex::token::action_set_token_admin()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                PRINT(" newadmin: ", newadmin.to_string(), "\n");

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_ASTA_1, sym_code).c_str());

                check( is_account( newadmin ), create_error_name1(ERROR_ASTA_2, newadmin).c_str());
                check(has_auth(vapaee::dex::contract) || has_auth(itr->admin), ERROR_ASTA_2);

                tokenstable.modify( *itr, same_payer, [&]( auto& a ){
                    a.admin = newadmin;
                });

                PRINT("vapaee::dex::token::action_set_token_admin() ...\n");
            }

            void action_set_token_data (const symbol_code & sym_code, uint64_t id, name action, string text, string link, name shownas) {
                PRINT("vapaee::dex::token::action_set_token_data()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                PRINT(" id: ", std::to_string((unsigned long) id), "\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" shownas: ", shownas.to_string(), "\n");
                PRINT(" text: ", text.c_str(), "\n");
                PRINT(" link: ", link.c_str(), "\n"); 

                tokens tokenstable(contract, contract.value);
                auto tkitr = tokenstable.find(sym_code.raw());
                check(tkitr != tokenstable.end(), "Token not registered. You must register it first calling addtoken action");
                name admin = tkitr->admin;
                check(has_auth(contract) || has_auth(admin), "only admin or token's admin can modify the token data");
                name ram_payer = admin;
                if (has_auth(contract)) {
                    ram_payer = contract;
                }

                require_auth( ram_payer );

                tokendata tokendatatable(contract, sym_code.raw());
                auto itr = tokendatatable.find(id);
                if (action == name("add")) {
                    tokendatatable.emplace( ram_payer, [&]( auto& a ){
                        a.id        = tokendatatable.available_primary_key();
                        a.shownas  = shownas;
                        a.text      = text;
                        a.link      = link;
                        a.date      = vapaee::dex::global::get_now_time_point_sec();
                    });
                    tokenstable.modify(*tkitr, same_payer, [&]( auto& a){
                        a.data++;
                    });
                } else {
                    check(itr != tokendatatable.end(), "No action can be performed on entry with this id because it does not exist");
                    if (action == name("remove")) {
                        tokendatatable.erase(*itr);
                        tokenstable.modify(*tkitr, same_payer, [&]( auto& a){
                            a.data--;
                            check(a.data >= 0, "Data inconsistency");
                        });
                    } else {
                        tokendatatable.modify(*itr, same_payer, [&](auto& a){
                            a.shownas  = shownas;
                            a.text      = text;
                            a.link      = link;
                            a.date      = vapaee::dex::global::get_now_time_point_sec();
                        });
                    }
                }
                
                PRINT("vapaee::dex::token::action_set_token_data() ...\n");            
            }

            // currencies ----------
            void aux_set_token_as_currency (const symbol_code & sym_code, uint64_t currency, name ram_payer) {
                PRINT("vapaee::dex::token::aux_set_token_as_currency()\n");
                PRINT(" sym_code:    ", sym_code.to_string(), "\n");
                PRINT(" currency: ", std::to_string((unsigned long)currency), "\n");
                PRINT(" ram_payer:    ", ram_payer.to_string(), "\n");

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_ASTAC_1, sym_code).c_str());

                // check if exists another token already registered with this currency number
                auto currency_index = tokenstable.get_index<"currency"_n>();
                auto citr = currency_index.find(currency);
                check(citr == currency_index.end(), create_error_string2(ERROR_ASTAC_2, std::to_string((int)citr->currency), std::to_string((int)currency)).c_str());

                // Set this token as currency
                tokenstable.modify( *itr, ram_payer, [&]( auto& a ){
                    a.currency = currency;
                });

                PRINT("vapaee::dex::token::aux_set_token_as_currency() ...\n");
            }

            void action_add_currency(const symbol_code & sym_code, string website, string brief) {
                PRINT("vapaee::dex::token::action_add_currency()\n");
                PRINT(" website: ", website.c_str(), "\n");
                PRINT(" brief: ", brief.c_str(), "\n");
                
                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_AATG_1, sym_code).c_str());

                // signature and ram payer
                name rampayer = itr->admin;
                if (has_auth(vapaee::dex::contract)) {
                    rampayer = vapaee::dex::contract;
                } else {
                    check(has_auth(itr->admin), create_error_name1(ERROR_AATG_2, itr->admin).c_str());
                }

                currencies table(vapaee::dex::contract, vapaee::dex::contract.value);
                

                // If this is the first time, we need first to create a dummie entry with the 0 currency number
                // which means that this token is not a currency
                uint64_t id = table.available_primary_key();
                if (id == 0) {
                    table.emplace(rampayer, [&]( auto& a ){
                        a.id         = id;
                        a.website    = "dummie";
                        a.brief      = "dummie";
                        a.currency   = eosio::symbol_code("NONE");
                    });
                    // increment id to 1
                    id = 1;
                }

                table.emplace(rampayer, [&]( auto& a ){
                    a.id         = id;
                    a.website    = website;
                    a.brief      = brief;
                    a.currency   = sym_code;
                });


                aux_set_token_as_currency(sym_code, id, rampayer);

                PRINT(" -> currencies.emplace() website:'", website, "' with id ", std::to_string((unsigned) id), "\n");

                PRINT("vapaee::dex::token::action_add_currency() ...\n");
            }

            void action_update_currency_data(uint64_t currency_id, string website, string brief) {
                PRINT("vapaee::dex::token::action_update_currency_data()\n");
                PRINT(" currency_id: ", std::to_string((unsigned) currency_id), "\n");
                PRINT(" website: ", website.c_str(), "\n");
                PRINT(" brief: ", brief.c_str(), "\n");

                currencies table(vapaee::dex::contract, vapaee::dex::contract.value);
                auto ptr = table.find(currency_id);
                check(ptr != table.end(), create_error_id1(ERROR_AUTG_3, currency_id).c_str());

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(ptr->currency.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_AATG_1, ptr->currency).c_str());

                // admin must exist
                check( is_account( itr->admin ), create_error_name1(ERROR_AUTG_1, itr->admin).c_str());

                // signature and ram payer
                name rampayer = itr->admin;
                if (has_auth(vapaee::dex::contract)) {
                    rampayer = same_payer;
                } else {
                    check(has_auth(itr->admin), create_error_name1(ERROR_AUTG_2, itr->admin).c_str());
                }
                
                // update table
                table.modify( *ptr, rampayer, [&](auto & a){
                    a.brief     = brief;
                    a.website   = website;
                });

                PRINT("vapaee::dex::token::action_update_currency_data() ...\n");
            }

        };     
    };
};
