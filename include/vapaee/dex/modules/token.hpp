#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/dao.hpp>

using namespace std;
#define TOKEN_GROUP_ZERO 0

namespace vapaee {
    namespace dex {
        using namespace utils;
        
        namespace token {

            // init ----------------

            void init() {
                PRINT("vapaee::dex::token::init()\n");

                PRINT(" action addtoken(TLOS)\n");
                string brief = "Telos is a networked ecosystem enabling visionary leaders and communities to work together to build a new global economy.\n\n\nTLOS is the main token in the Telos blockchain.";
                string briefg = "This is the main group of all standard tokens deployed on Telos Network. Any token included in this group can be traded with TLOS of any other token declared as currency in Telos DEX.";
                string banner = "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/uploads/telos-banner.jpg";
                string icon = "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/telos.png";
                string iconlg = "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/telos-lg.png";
                string group = "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/uploads/telos-token-group.png";
                
                string pcontact = "https://telosfoundation.io";
                string gcontact = "https://t.me/HelloTelos";

                action(
                    permission_level{contract,name("active")},
                    contract,
                    name("addtoken"),
                    std::make_tuple(
                        name("eosio.token"),                   // name contract
                        symbol_code("TLOS"),                   // const symbol_code & symbol
                        (uint8_t)4,                            // uint8_t precision
                        contract,                            // name admin
                        string("Telos"),                       // string title
                        string("https://telosfoundation.io"),  // string website
                        brief,                                 // string brief
                        banner,                                // string banner
                        icon,                                  // string icon
                        iconlg,                                // string iconlg
                        pcontact,                              // string pcontact
                        gcontact,                              // string gcontact
                        true                                   // bool tradeable
                    )
                ).send();

                PRINT(" action addtnkgroup(0)\n");
                action(
                    permission_level{contract,name("active")},
                    contract,
                    name("addtnkgroup"),
                    std::make_tuple(
                        contract,
                        string("Telos DEX main market group"),
                        string("https://vapaee.io/exchange/tokens"),
                        briefg,                                // string brief
                        banner,
                        group
                    )
                ).send();

                PRINT(" action setcurrency(TLOS)\n");
                action(
                    permission_level{contract,name("active")},
                    contract,
                    name("setcurrency"),
                    std::make_tuple(symbol_code("TLOS"), true, (uint64_t)0)
                ).send();


                PRINT("vapaee::dex::token::init() ...\n");
            }

            // tokens ---------------- 
            
            void action_add_token(name contract, const symbol_code & sym_code, uint8_t precision, name admin) {
                PRINT("vapaee::dex::token::action_add_token()\n");
                PRINT(" contract: ", contract.to_string(), "\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                PRINT(" precision: ", std::to_string((unsigned) precision), "\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                
                // check if tokens existe in token contract account name
                stats statstable( contract, sym_code.raw() );
                auto token_itr = statstable.find( sym_code.raw() );
                check(token_itr != statstable.end(), create_error_symcode1(ERROR_AAT_1, sym_code).c_str());
                
                check(has_auth(contract) || has_auth(contract) || has_auth(token_itr->issuer), "only token contract or issuer can add this token to DEX" );

                tokens tokenstable(contract, contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr == tokenstable.end(), create_error_symcode1(ERROR_AAT_2, sym_code).c_str());
                tokenstable.emplace( admin, [&]( auto& a ){
                    a.contract  = contract;
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
                });
                PRINT(" -> tokenstable.emplace() OK\n");

                if (vapaee::utils::SYS_TKN_CONTRACT != contract && vapaee::utils::SYS_TKN_CODE != sym_code) {
                    // this token is not TLOS

                    // charge feepayer for the fees to pay telos.decide ballot service
                    asset quantity = vapaee::dex::global::get().regcost;
                    asset qextended = aux_extend_asset(quantity);
                    PRINT(" -> charging the admin (",admin.to_string(),") with regcost (",qextended.to_string(),")\n");
                    vapaee::dex::deposit::aux_substract_deposits(admin, qextended);
                    
                    // send fees to 
                    PRINT(" -> transfer() ", quantity.to_string(), " to ", vapaee::dex::dao::saving, "\n");
                    action(
                        permission_level{contract,name("active")},
                        vapaee::utils::SYS_TKN_CONTRACT,
                        name("transfer"),
                        std::make_tuple(contract, vapaee::dex::dao::saving, quantity, create_error_symcode1("Telos DEX fees for registering new token:", sym_code))
                    ).send();

                }

                vapaee::dex::record::aux_register_event(admin, name("new.token"), string("token|") + sym_code.to_string() + " (" + contract.to_string() + ")");

                PRINT("vapaee::dex::token::action_add_token() ...\n");
            }

            void action_update_token_info(const symbol_code & sym_code, string title, string website, string brief, string banner, string icon, string iconlg, string pcontact, string gcontact, vector<uint64_t> groups, bool tradeable) {
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

                tokens tokenstable(contract, contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_AUTI_1, sym_code).c_str());
                name admin = itr->admin;
                check(has_auth(contract) || has_auth(admin), ERROR_AUTI_2);

                // is it blacklisted?
                check(!vapaee::dex::dao::aux_is_token_blacklisted(itr->symbol, itr->contract), 
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
                });

                PRINT("vapaee::dex::token::action_update_token_info() ...\n");
            }

            void action_change_groups_for_a_token(const symbol_code & sym_code, vector<uint64_t> groups) {
                PRINT("vapaee::dex::token::action_change_groups_for_a_token()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                for (int i=0; i<groups.size(); i++) {
                    PRINT(" groups[",i,"]: ", std::to_string((unsigned long) groups[i]), "\n");
                }

                tokens tokenstable(contract, contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_AUTI_1, sym_code).c_str());
                name admin = itr->admin;
                check(has_auth(contract) || has_auth(admin), ERROR_AUTI_2);

                // is it blacklisted?
                check(!vapaee::dex::dao::aux_is_token_blacklisted(itr->symbol, itr->contract), 
                    create_error_symcode1(ERROR_AUTI_3, itr->symbol).c_str());
                
                tokenstable.modify( *itr, same_payer, [&]( auto& a ){
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

                tokens tokenstable(contract, contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_ASTAC_1, sym_code).c_str());

                tokengroups groupstable(contract, contract.value);
                auto ptr = groupstable.find(token_group);
                check(ptr != groupstable.end(), create_error_id1(ERROR_ASTAC_2, token_group).c_str());
                
                if (TOKEN_GROUP_ZERO == token_group) {
                    check(has_auth(contract), ERROR_ASTAC_3);
                    PRINT(" -> tokens.modify().currency: ", itr->symbol.to_string(), "\n");
                    tokenstable.modify( *itr, same_payer, [&]( auto& a ){
                        a.currency = is_currency;
                    });
                } else {
                    check(has_auth(ptr->admin), create_error_name1(ERROR_ASTAC_4, ptr->admin).c_str());
                    check(has_auth(itr->admin), create_error_name1(ERROR_ASTAC_5, itr->admin).c_str());
                }

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

                tokens tokenstable(contract, contract.value);
                auto itr = tokenstable.find(sym_code.raw());
                check(itr != tokenstable.end(), create_error_symcode1(ERROR_ASTA_1, sym_code).c_str());

                check( is_account( newadmin ), create_error_name1(ERROR_ASTA_2, newadmin).c_str());
                check(has_auth(contract) || has_auth(itr->admin), ERROR_ASTA_2);

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

            void action_edit_token_event(const symbol_code & sym_code, name event, name action, name receptor) {
                PRINT("vapaee::dex::token::action_edit_token_event()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                PRINT(" event: ", event.to_string(), "\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" receptor: ", receptor.to_string(), "\n");


                bool event_ok = false;
                if (!event_ok && event == name("withdraw"))     event_ok = true;
                if (!event_ok && event == name("deposit"))      event_ok = true;
                if (!event_ok && event == name("swapdeposit"))  event_ok = true;
                if (!event_ok && event == name("order"))        event_ok = true;
                if (!event_ok && event == name("cancel"))       event_ok = true;
                if (!event_ok && event == name("deal"))         event_ok = true;

                if (!event_ok) {
                    string error = string("'") + event.to_string() + "' is not a valid event ('withdraw', 'deposit', 'swapdeposit', 'order', 'cancel', 'deal')";
                    check(event_ok, error.c_str());
                }
                
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

                tokenevents tokeneventstable(contract, sym_code.raw());
                auto itr = tokeneventstable.find(event.value);
                if (action == name("add")) {
                    check(itr == tokeneventstable.end(), "The event is already registered. User action 'modify' instead of 'add'");
                    tokeneventstable.emplace( ram_payer, [&]( auto& a ){
                        a.event     = event;
                        a.receptor  = receptor;
                    });
                } else {
                    check(itr != tokeneventstable.end(), "No action can be performed on entry with this id because it does not exist");
                    if (action == name("remove")) {
                        tokeneventstable.erase(*itr);
                    } else {
                        tokeneventstable.modify(*itr, same_payer, [&](auto& a){
                            a.receptor  = receptor;
                        });
                    }
                }

                PRINT("vapaee::dex::token::action_edit_token_event()...\n");
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
                if (has_auth(contract)) {
                    rampayer = contract;
                } else {
                    check(has_auth(admin), create_error_name1(ERROR_AATG_2, admin).c_str());
                }

                tokengroups table(contract, contract.value);
                
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
                if (has_auth(contract)) {
                    rampayer = same_payer;
                } else {
                    check(has_auth(admin), create_error_name1(ERROR_AUTG_2, admin).c_str());
                }
                
                // update table
                tokengroups table(contract, contract.value);
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
