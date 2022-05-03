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

            name get_asset_token_contract(asset quantity) {

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(quantity.symbol.code().raw());

                if (itr != tokenstable.end()) {
                    return itr->contract;
                } else {
                    return name();
                }
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

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr == tokenstable.end(), create_error_symcode1(ERROR_AAT_2, sym_code).c_str());
                tokenstable.emplace( admin, [&]( auto& a ){
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
                vector<uint64_t> groups,
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
                for (int i=0; i<groups.size(); i++) {
                    PRINT(" groups[",i,"]: ", std::to_string((unsigned long) groups[i]), "\n");
                }
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
                    a.groups.clear();
                    a.groups.assign(groups.begin(), groups.end());
                    a.title     = title;
                    a.website   = website;
                    a.brief     = brief;
                    a.banner    = banner;
                    a.icon      = icon;
                    a.iconlg    = iconlg;
                    a.pcontact  = pcontact;
                    a.gcontact  = gcontact;
                    a.tradeable = tradeable;
                    a.date      = vapaee::dex::global::get_now_time_point_sec();
                    a.stable    = stable;
                });

                PRINT("vapaee::dex::token::action_update_token_info() ...\n");
            }

            void action_change_groups_for_a_token(const symbol_code & sym_code, vector<uint64_t> groups) {
                PRINT("vapaee::dex::token::action_change_groups_for_a_token()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                for (int i=0; i<groups.size(); i++) {
                    PRINT(" groups[",i,"]: ", std::to_string((unsigned long) groups[i]), "\n");
                }

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_AUTI_1, sym_code).c_str());
                name admin = itr->admin;
                name ram_payer = admin;
                if (has_auth(contract)) {
                    ram_payer = contract;
                }
                require_auth( ram_payer );
                
                
                // PRINT(" -> check(has_auth(",vapaee::dex::contract.to_string(),"), ERROR_ACGFAT_2); ","\n");
                check(has_auth(vapaee::dex::contract), ERROR_ACGFAT_2);

                // is it blacklisted?
                check(!vapaee::dex::security::aux_is_token_blacklisted(itr->symbol, itr->contract), 
                    create_error_symcode1(ERROR_AUTI_3, itr->symbol).c_str());

                // PRINT(" -> tokenstable.modify(... ","\n");
                tokenstable.modify( *itr, ram_payer, [&]( auto& a ){
                    a.groups.clear();
                    a.groups.assign(groups.begin(), groups.end());
                    a.date = vapaee::dex::global::get_now_time_point_sec();
                });                                
                PRINT("vapaee::dex::token::action_change_groups_for_a_token() ...\n");
            }

            void action_set_token_as_currency (const symbol_code & sym_code, bool is_currency, uint64_t token_group) {
                PRINT("vapaee::dex::token::action_set_token_as_currency()\n");
                PRINT(" sym_code:    ", sym_code.to_string(), "\n");
                PRINT(" is_currency: ", std::to_string(is_currency), "\n");
                PRINT(" token_group: ", std::to_string((unsigned long)token_group), "\n");

                tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_ASTAC_1, sym_code).c_str());

                tokengroups groupstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto ptr = groupstable.find(token_group);
                check(ptr != groupstable.end(), create_error_id1(ERROR_ASTAC_2, token_group).c_str());
                
                PRINT(" -> check(has_auth(",vapaee::dex::contract.to_string(),"), ERROR_ASTAC_3); ","\n");
                check(has_auth(vapaee::dex::contract), ERROR_ASTAC_3);

                // All groups will have only one currency token that will be marked as such.
                tokenstable.modify( *itr, same_payer, [&]( auto& a ){
                    a.currency = is_currency;
                });


                bool belongs_to_currencies = std::find(ptr->currencies.begin(), ptr->currencies.end(), sym_code) != ptr->currencies.end();
                PRINT(" -> belongs_to_currencies: ", std::to_string(belongs_to_currencies),"\n");
                if (is_currency) {
                    check(!belongs_to_currencies,
                        create_error_symcode1(ERROR_ASTAC_6, sym_code).c_str());
                } else {
                    check(belongs_to_currencies,
                        create_error_symcode1(ERROR_ASTAC_7, sym_code).c_str());
                }

                if (is_currency) {
                    PRINT(" -> tokengroups.modify() adding ",sym_code.to_string(),"\n");
                    groupstable.modify(*ptr, same_payer, [&](auto &a){
                        a.currencies.push_back(sym_code);
                    });
                } else {
                    PRINT(" -> tokengroups.modify() removing ",sym_code.to_string(),"\n");
                    groupstable.modify(*ptr, same_payer, [&](auto &a){
                        std::vector<symbol_code> newlist;
                        std::copy_if (a.currencies.begin(), a.currencies.end(), std::back_inserter(newlist), [&](symbol_code sym){return sym!=sym_code;} );
                        a.currencies = newlist;
                    });
                }

                PRINT("vapaee::dex::token::action_set_token_as_currency() ...\n");
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

            // token groups ----------

            void action_add_token_group(name admin, string title, string website, string brief, string banner, string thumbnail) {
                PRINT("vapaee::dex::token::action_add_token_group()\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" website: ", website.c_str(), "\n");
                PRINT(" brief: ", brief.c_str(), "\n");
                PRINT(" banner: ", banner.c_str(), "\n");
                PRINT(" thumbnail: ", thumbnail.c_str(), "\n");

                // admin must exist
                check( is_account( admin ), create_error_name1(ERROR_AATG_1, admin).c_str());

                // signature and ram payer
                name rampayer = admin;
                if (has_auth(vapaee::dex::contract)) {
                    rampayer = vapaee::dex::contract;
                } else {
                    check(has_auth(admin), create_error_name1(ERROR_AATG_2, admin).c_str());
                }

                tokengroups table(vapaee::dex::contract, vapaee::dex::contract.value);
                
                uint64_t id = table.available_primary_key();
                table.emplace(rampayer, [&]( auto& a ){
                    a.id         = id;
                    a.admin      = admin;
                    a.title      = title;
                    a.website    = website;
                    a.brief      = brief;
                    a.banner     = banner;
                    a.thumbnail  = thumbnail;
                });

                PRINT(" -> tokengroups.emplace() ", title, " with id ", std::to_string((unsigned) id), "\n");

                PRINT("vapaee::dex::token::action_add_token_group() ...\n");
            }

            void action_update_token_group(uint64_t group_id, name admin, string title, string website, string brief, string banner, string thumbnail) {
                PRINT("vapaee::dex::token::action_update_token_group()\n");
                PRINT(" group_id: ", std::to_string((unsigned) group_id), "\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" website: ", website.c_str(), "\n");
                PRINT(" brief: ", brief.c_str(), "\n");
                PRINT(" banner: ", banner.c_str(), "\n");
                PRINT(" thumbnail: ", thumbnail.c_str(), "\n");

                // admin must exist
                check( is_account( admin ), create_error_name1(ERROR_AUTG_1, admin).c_str());

                // signature and ram payer
                name rampayer = admin;
                if (has_auth(vapaee::dex::contract)) {
                    rampayer = same_payer;
                } else {
                    check(has_auth(admin), create_error_name1(ERROR_AUTG_2, admin).c_str());
                }
                
                // update table
                tokengroups table(vapaee::dex::contract, vapaee::dex::contract.value);
                auto ptr = table.find(group_id);
                check(ptr != table.end(), create_error_id1(ERROR_AUTG_3, group_id).c_str());

                table.modify( *ptr, rampayer, [&](auto & a){
                    a.admin     = admin;
                    a.title     = title;
                    a.website   = website;
                    a.brief     = brief;
                    a.banner    = banner;
                    a.thumbnail = thumbnail;
                });

                PRINT(" -> tokengroups.modify(): \n");

                PRINT("vapaee::dex::token::action_update_token_group() ...\n");
            }

        };     
    };
};
