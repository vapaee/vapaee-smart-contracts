#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/tables.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/pay/modules/rex.hpp>
#include <vapaee/pay/modules/vip.hpp>
#include <vapaee/token/modules/wrapper.hpp>
#include <vapaee/pool/modules/util.hpp>

namespace vapaee {
    namespace pay {
        namespace hub {

            
            // void check_payhub_recipients_integrity(const payhubs_table& payhub, const char * error_code);

            inline name get_self() {
                return vapaee::pay::contract;
            }

            // PayHub pocket balances

            bool does_pocket_exist(uint64_t payhub_id, const symbol_code& token) {
                // get current supply for this token
                asset supply = vapaee::token::wrapper::get_token_supply(token, "ERR-DPE-01");
                asset zero_balance = asset(0, supply.symbol);

                paypockets_table fund;
                fund.balance = zero_balance;
                fund.payhub = payhub_id;

                paypockets funds_tbl( get_self(), get_self().value );
                auto index = funds_tbl.get_index<name("pocket")>();
                auto to = index.find( fund.by_pocket()  );
                return to != index.end();
            }

            void opn_payhub_balance(uint64_t payhub_id, const symbol_code& token, const name& ram_payer) {
                // get current supply for this token
                asset supply = vapaee::token::wrapper::get_token_supply(token, "ERR-OPB-1");
                asset zero_balance = asset(0, supply.symbol);

                paypockets_table fund;
                fund.balance = zero_balance;
                fund.payhub = payhub_id;

                paypockets funds_tbl( get_self(), get_self().value );
                auto index = funds_tbl.get_index<name("pocket")>();
                auto to = index.find( fund.by_pocket()  );
                if( to == index.end() ) {
                    funds_tbl.emplace(ram_payer, [&](auto &a){
                        a.id      = funds_tbl.available_primary_key();
                        a.balance = zero_balance;
                        a.payhub  = payhub_id;
                    });
                }
            }

            void add_payhub_balance(uint64_t payhub_id, const asset& quantity) {
                asset zero_balance = asset(0, quantity.symbol);
                paypockets_table fund;
                fund.balance = zero_balance;
                fund.payhub = payhub_id;

                paypockets funds_tbl( get_self(), get_self().value );
                auto index = funds_tbl.get_index<name("pocket")>();
                auto to = index.lower_bound( fund.by_pocket()  );

                // DEBUG: this iterates over all pokets found and show them all (if more than one)
                for (
                    to = index.begin();
                    to != index.end();
                    to++
                ) {
                    checksum256 fund_hash = fund.by_pocket();
                    checksum256 to_hash = to->by_pocket();

                    PRINT("to: ", to->to_string()," ",
                        vapaee::utils::to_hex(&to_hash, sizeof(to_hash)), " ",
                        fund.to_string(), " ",
                        vapaee::utils::to_hex(&fund_hash, sizeof(fund_hash)),
                    "\n");
                }

                to = index.lower_bound( fund.by_pocket()  );
                check(to != index.end(), create_error_string1(
                    "ERR-APHB-01: PayHub pocket not found: ",
                    fund.to_string()
                ).c_str());

                funds_tbl.modify(*to, same_payer, [&]( auto& a ) {
                    PRINT("vapaee::pay::hub::add_payhub_balance() a.balance: ", a.balance.to_string(), " quantity: ", quantity.to_string(), "\n");                    
                    a.balance += quantity;
                    a.in = vapaee::dex::global::get_now_time_point_sec();
                });
            }

            void sub_payhub_balance(uint64_t payhub_id, const asset& quantity) {
                
                PRINT("vapaee::pay::hub::sub_payhub_balance()\n");
                PRINT(" payhub_id: ", std::to_string((long)payhub_id), "\n");                
                PRINT(" quantity: ", quantity.to_string(), "\n");
                asset zero_balance = asset(0, quantity.symbol);
                paypockets_table fund;
                fund.balance = zero_balance;
                fund.payhub = payhub_id;

                paypockets funds_tbl( get_self(), get_self().value );
                auto index = funds_tbl.get_index<name("pocket")>();
                auto from = index.find( fund.by_pocket()  );

                check(from != index.end(), create_error_string1(
                    "ERR-SPHB-01: PayHub pocket not found: ",
                    fund.to_string()
                ).c_str());

                check( from->balance.amount >= quantity.amount, 
                    create_error_string3("ERR-SPHB-02: not enough balance: (pocket, balance, debit)",
                        fund.to_string(), from->balance.to_string(), quantity.to_string()) );
                
                funds_tbl.modify( *from, same_payer, [&]( auto& a ) {
                    a.balance -= quantity;
                    a.out = vapaee::dex::global::get_now_time_point_sec();
                });
            }

            asset get_payhub_balance(uint64_t payhub_id, const symbol_code& token) {
                PRINT("vapaee::pay::hub::get_payhub_balance()\n");
                PRINT(" payhub_id: ", std::to_string((long)payhub_id), "\n");                
                PRINT(" token: ", token.to_string(), "\n");

                asset zero_balance = asset(0, symbol(token,0));
                paypockets_table fund;
                fund.balance = zero_balance;
                fund.payhub = payhub_id;
                
                paypockets funds_tbl( get_self(), get_self().value );
                auto index = funds_tbl.get_index<name("pocket")>();
                auto to = index.find( fund.by_pocket()  );
                
                check(to != index.end(), create_error_string1(
                    "ERR-GPHB-01: PayHub pocket not found: ",
                    fund.to_string()
                ).c_str());

                return to->balance;
            }             

            // auxiliar ---
            uint64_t extract_payhub_id(const string& target) {
                return std::atol(target.c_str());
            }

            bool get_payhub_for_id(
                bool create,
                uint64_t payhub_id,
                payhubs_table& payhub,
                const name& ram_payer,
                const char * error_code
            ) {
                if (create) PRINT("vapaee::pay::hub::get_payhub_for_id(true,",
                    ( payhub_id == MAX_VALUE ? "MAX_VALUE" : std::to_string((long)payhub_id) ),
                    ", ", payhub.alias.c_str(), ")\n");

                payhubs hubs_table(get_self(), get_self().value);
                auto hub_ptr = hubs_table.find(payhub_id);

                if (hub_ptr == hubs_table.end() && !create)  {
                    if (error_code) {
                        check(false, 
                            create_error_id1(
                                string(string(error_code) + " > ERR-GPFI-01: the payhubs entry for the id was not found. (payhub_id): ").c_str(),
                                payhub_id
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (hub_ptr == hubs_table.end() && create) {
                    hubs_table.emplace(ram_payer, [&](auto &a){
                        payhub.id     = hubs_table.available_primary_key();
                        a.id          = payhub.id;
                        a.alias       = payhub.alias;
                        a.admin       = payhub.admin;
                        a.main_pocket = payhub.main_pocket;
                        a.billing_to  = payhub.billing_to;
                        a.recipients.clear();
                        for (int i=0; i<payhub.recipients.size(); i++)
                            a.recipients.push_back(payhub.recipients[i]); 
                    });
                    return true;
                };

                if (hub_ptr != hubs_table.end() && create) {
                    hubs_table.modify(hub_ptr, ram_payer, [&](auto &a){
                        a.alias       = payhub.alias;
                        a.admin       = payhub.admin;
                        a.main_pocket = payhub.main_pocket;
                        a.billing_to  = payhub.billing_to;
                        a.recipients.clear();
                        for (int i=0; i<payhub.recipients.size(); i++)
                            a.recipients.push_back(payhub.recipients[i]);
                    });

                    return true;
                };
                
                payhub.id          = hub_ptr->id;
                payhub.alias       = hub_ptr->alias;
                payhub.admin       = hub_ptr->admin;
                payhub.main_pocket = hub_ptr->main_pocket;
                payhub.billing_to  = hub_ptr->billing_to;
                payhub.recipients.clear();
                for (int i=0; i<hub_ptr->recipients.size(); i++)
                    payhub.recipients.push_back(hub_ptr->recipients[i]);

                return true;
            }

            bool get_payhub_for_alias(
                bool create,
                const string& alias,
                payhubs_table& payhub,
                const char * error_code
            ) {
                if (create) PRINT("vapaee::pay::hub::get_payhub_for_alias(true,", alias.c_str(), ", ", payhub.alias.c_str(), ")\n");
                payhubs hubs_table(get_self(), get_self().value);
                auto index = hubs_table.get_index<name("alias")>();
                auto itr = index.find(vapaee::utils::hash(alias));

                if (itr == index.end() && !create) {
                    if (error_code) {
                        check(false,
                            create_error_string1(
                                string(string(error_code) + " > ERR-GPFA-01: the payhubs entry for alias was not found. (alias): ").c_str(),
                                alias
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                uint64_t id = MAX_VALUE;
                if (itr != index.end()) {
                    id = itr->id;
                }
                return get_payhub_for_id(create, id, payhub, get_self(), error_code);
            }

            int parse_payhub_target(const string& target, payhub_target& pay_target) {
                PRINT("vapaee::pay::hub::parse_payhub_target()\n");
                PRINT(" target: ", target.c_str(), "\n");
                PRINT(" pay_target: ", pay_target.to_string(), "\n");
                bool fail, success;
                name ram_payer = get_self();
                payhubs_table payhub;

                recipient_info recipient;
                pay_target.target = target;
                pay_target.payhub = MAX_VALUE;

                // 1 - it is a name and exists the account -> Telos account
                PRINT(" 1 - it is a name and exists the account -> Telos account\n");
                name account;
                fail = vapaee::utils::get_name_from_string(target, account);
                if (!fail && is_account(account)) {
                    pay_target.account = account;
                    pay_target.type = TARGET_ACCOUNT;

                    bool found = get_payhub_for_alias(false, account.to_string(), payhub, NULL);
                    if (found) {
                        pay_target.payhub = payhub.id;
                        pay_target.type = TARGET_NAME;
                        PRINT(" Existe un alias\n");
                    }
                    PRINT(" >> pay_target: ", pay_target.to_string(), "\n");
                    
                    return pay_target.type;
                }

                // 2 - it is a number -> PayHub(id)
                PRINT(" 2 - it is a number -> PayHub(id)\n");
                bool is_num = vapaee::utils::is_natural(target);
                if (is_num) {
                    uint64_t payhub_id = extract_payhub_id(target);
                    get_payhub_for_id(false, payhub_id, payhub, ram_payer, "ERR-PPT-01");
                    pay_target.payhub = payhub_id;
                    pay_target.type = TARGET_PAYHUB;
                    return pay_target.type;
                }
                
                // 3 - it is a "<symbol_code>-<name>" -> REX pool (token & pool_id)
                PRINT(" 3 - it is a <symbol_code>-<name> -> REX pool (token & pool_id)\n");
                pool_id pool;
                success = vapaee::pay::vip::extract_pool_id(target, pool);
                if (success) {
                    pay_target.pool = pool;
                    pay_target.type = TARGET_POOL;
                    stakepool_table stakepool;
                    vapaee::pay::rex::get_stakepool_for_pool_id(false, pool.token, pool.id, stakepool, ram_payer, "ERR-PPT-02");

                    return pay_target.type;
                }

                // 4 - it is a simple string -> must exist a (vip) name -> PayHub
                PRINT(" 4 - it is a simple string -> must exist a (vip) name -> PayHub\n");
                names_table vip_name;
                vip_name.alias = target;
                
                // Check exists the vip name (alias)
                vapaee::pay::vip::get_alias(false, vip_name, ram_payer, "ERR-PPT-03");

                // Check exissts the payhub with that alias
                get_payhub_for_alias(false, vip_name.alias, payhub, "ERR-PPT-04");
                pay_target.type = TARGET_NAME;
                pay_target.payhub = payhub.id;

                return pay_target.type;
            }
            
            void check_payhub_recipients_integrity(const payhubs_table& payhub, const char * error_code) {
                // Check if recipients sum exactly one
                check(payhub.recipients.size() > 0, 
                    string(string(error_code) + ": PayHub recipients list can't be empty").c_str()
                );
                // Check if recipients sum exactly one
                check(payhub.recipients.size() <= 10, 
                    string(string(error_code) + ": PayHub can't manage more than 10 recipients at once. Try to use tree structures").c_str()
                );
                
                asset sum = asset(0, payhub.recipients[0].part.symbol);
                payhub_target pay_target;
                for (int i=0; i<payhub.recipients.size(); i++) {
                    sum += payhub.recipients[i].part;
                    parse_payhub_target(payhub.recipients[i].target, pay_target);
                    // string(string(error_code) " > ERR-CPRI-01: recipient[" + string(i) +"].target ("+payhub.recipients[i].target+") is not a valid PayHub." ).c_str()
                }
                
                asset one = asset(ipow(10, sum.symbol.precision()), sum.symbol);

                if (one.amount != sum.amount) {
                    for (int i=0; i<payhub.recipients.size(); i++) {
                        PRINT("payhub.recipients[",std::to_string(i),"]: ", payhub.recipients[i].to_string(), "\n");
                    }
                }
                PRINT(" sum: ", sum.to_string(), "\n");
                PRINT(" one: ", one.to_string(), "\n");

                check(one.amount == sum.amount,
                    create_error_asset2(
                        string(string(error_code) + " > ERR-CPRI-02: the payhubs entry for the id was not found. (payhub_id): ").c_str(),
                        one,
                        sum
                    ).c_str()
                );
            }


            // actions ---

            // admin registrates a new payhub account with a list of recipients and their participation.
            // If the admin wants to give the account an alias it must be already in names table
            void action_newpayhub(
                const name& admin,
                const string& alias,
                const std::vector<std::tuple<asset,string>>& recipients,
                const std::vector<symbol_code>& pockets,
                const name& billing_to
            ) {
                PRINT("vapaee::pay::hub::action_newpayhub()\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" alias: ", alias.c_str(), "\n");
                int i;
                i=0; for(const auto& c: recipients) {
                    PRINT(" recipients[",i++,"]: ", std::get<0>(c).to_string(), " - ", std::get<1>(c),  "\n");
                }
                i=0; for(const auto& c: pockets) {
                    PRINT(" pockets[",i++,"]: ", c.to_string(), "\n");
                }
                PRINT(" billing_to: ", billing_to.to_string(), "\n");

                check(pockets.size() > 0, "ERR-NP-01: PayHub must have at least one pocket.");
                symbol_code main_pocket = pockets[0];

                // require admin auth
                require_auth(admin);
                name ram_payer = admin;

                payhubs_table payhub;
                payhub.admin       = admin;
                payhub.alias       = alias;
                payhub.billing_to  = billing_to;
                payhub.main_pocket = main_pocket;
                for(const auto& c: recipients) {
                    recipient_info info;
                    info.part = std::get<0>(c); 
                    info.target = std::get<1>(c);
                    payhub.recipients.push_back(info);
                }

                // we must check recipients integrity.
                check(payhub.recipients.size() > 0, "ERR-ANPH-02: PayHub must have at least one recipient");
                check_payhub_recipients_integrity(payhub, "ERR-ANPH-03");
                
                if (alias.length() > 0) {

                    // let's check if the alias is an account name or if it is a vip name
                    name account;
                    int error = vapaee::utils::get_name_from_string(alias, account);
                    if (error == TYPERR_NO_ERROR) {
                        // it is an account. 
                        check(is_account(account), "ERR-ANPH-04");

                        // is the admin
                        check(account == admin, "ERR-ANPH-05");

                        // has only one recipient and it is the admin
                        check(payhub.recipients.size() == 1, "ERR-ANPH-06");
                        error = vapaee::utils::get_name_from_string(payhub.recipients[0].target, account);
                        check(error == TYPERR_NO_ERROR && account == admin,
                            create_error_string2(
                                "ERR-ANPH-07: alias owner does not match action signer. (recipients[0].target, admin): ",
                                payhub.recipients[0].target,
                                admin.to_string()
                            ).c_str()
                        );

                        payhub.alias = admin.to_string();

                        PRINT(" PASAMOS POR ACA --> payhub.alias: ", payhub.alias.c_str(), "\n");

                    } else {
                        // it comes with a vip name as an alias. We must check it first.
                        names_table vip_name;
                        vip_name.alias = alias;          
                        vapaee::pay::vip::get_alias(false, vip_name, ram_payer, "ERR-ANPH-08");
                        check(admin == vip_name.owner, create_error_name2(
                            "ERR-ANPH-09: alias owner does not match action signer. (owner, signer): ",
                            vip_name.owner,
                            admin
                        ).c_str());                        
                    }

                    PRINT(" --> payhub: ", payhub.to_string(), "\n");

                    get_payhub_for_alias(true, payhub.alias, payhub, "ERR-ANPH-10");
                } else {
                    get_payhub_for_id(true, MAX_VALUE, payhub, ram_payer, "ERR-ANPH-11");
                }

                for (const auto& tkn: pockets) {
                    opn_payhub_balance(payhub.id, tkn, ram_payer);                    
                }
                
            }

            // admin updates payhub recipients
            void action_updatehub(
                const name& admin,
                uint64_t payhub_id,
                const std::vector<std::tuple<asset,string>>& recipients,
                const std::vector<symbol_code>& pockets,
                const name& billing_to
            ) {
                PRINT("vapaee::pay::hub::action_updatehub()\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" payhub_id: ", std::to_string((long)payhub_id), "\n");
                int i;
                i=0; for(const auto& c: recipients) {
                    PRINT(" recipients[",i++,"]: ", std::get<0>(c).to_string(), " - ", std::get<1>(c),  "\n");
                }
                i=0; for(const auto& c: pockets) {
                    PRINT(" pockets[",i++,"]: ", c.to_string(), "\n");
                }
                
                PRINT(" billing_to: ", billing_to.to_string(), "\n");

                check(pockets.size() > 0, "ERR-NP-01: PayHub must have at least one pocket.");
                symbol_code main_pocket = pockets[0];

                // require admin auth
                require_auth(admin);
                name ram_payer = admin;
                
                payhubs_table payhub;
                get_payhub_for_id(false, payhub_id, payhub, ram_payer, "ERR-AUH-01");

                // only if the alias is not an account name, we allow to update the recipients
                name account;
                int error = vapaee::utils::get_name_from_string(payhub.alias, account);
                if (error != TYPERR_NO_ERROR) {
                    payhub.recipients.clear();
                    for(const auto& c: recipients) {
                        recipient_info info;
                        info.part = std::get<0>(c);
                        info.target = std::get<1>(c);
                        payhub.recipients.push_back(info);
                    }
                    check_payhub_recipients_integrity(payhub, "ERR-AUH-02");
                }

                payhub.billing_to = billing_to;
                
                get_payhub_for_id(true, payhub_id, payhub, ram_payer, "ERR-AUH-03");

                for (const auto& tkn: pockets) {
                    opn_payhub_balance(payhub.id, tkn, ram_payer);
                }
            }

            // perform payment ------------
            void pay_to_account(const asset& quantity, const name& target, const string& memo) {
                action(
                    permission_level{get_self(), "active"_n},
                    vapaee::dex::utils::get_contract_for_token(quantity.symbol.code()),
                    name("transfer"), 
                    make_tuple(get_self(), target, quantity, memo)
                ).send();

            }
            
            void pay_to_pool(const asset& quantity, const pool_id& pool) {
                action(
                    permission_level{get_self(), "active"_n},
                    get_self(),
                    name("droponpool"), 
                    make_tuple(quantity, pool.id)
                ).send();
            }
            
            void pay_to_payhub(const asset& quantity, uint64_t payhub_id) {
                PRINT("vapaee::pay::hub::pay_to_payhub()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" payhub_id: ", std::to_string((long)payhub_id), "\n");
                payhubs_table payhub;
                get_payhub_for_id(false, payhub_id, payhub, get_self(), "ERR-PTPH-01");
                
                // add to pocket balance
                add_payhub_balance(payhub_id, quantity);
            }
            
            void pay_to_target(const asset& quantity, string& target) {
                PRINT("vapaee::pay::hub::pay_to_target()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" target: ", target.c_str(), "\n");

                payhub_target pay_target;
                parse_payhub_target(target, pay_target);

                string memo = string("");

                switch(pay_target.type) {
                    case TARGET_ACCOUNT: {
                        pay_to_account(quantity, pay_target.account, memo);
                        break;
                    }
                    case TARGET_PAYHUB: {}
                    case TARGET_NAME: {
                        pay_to_payhub(quantity, pay_target.payhub);
                        break;
                    }
                    case TARGET_POOL: {
                        pay_to_pool(quantity, pay_target.pool);
                        break;
                    }
                    default: {
                        check(false, create_error_string1(
                            "ERR-PP-01: was not posible to parse target",
                            target
                        ).c_str());
                    }
                }                
            }

            void move_pocket(uint64_t payhub_id, const symbol_code& token) {
                PRINT("vapaee::pay::hub::move_pocket()\n");
                PRINT(" payhub_id: ", std::to_string((long)payhub_id), "\n");
                PRINT(" token: ", token.to_string(), "\n");

                asset balance = get_payhub_balance(payhub_id, token);
                name ram_payer = get_self();

                payhubs_table payhub;
                get_payhub_for_id(false, payhub_id, payhub, ram_payer, "ERR-DPM-01");

                asset quantity = asset(0, balance.symbol);
                asset remaining = balance;
                asset sum = asset(0, payhub.recipients[0].part.symbol);
                asset part;
                string target;

                PRINT(" > balance: ", balance.to_string(), "\n");
                if (remaining.amount > 0) {
                    for (int i=0; i<payhub.recipients.size(); i++) {
                        target = payhub.recipients[i].target;
                        part = payhub.recipients[i].part;

                        quantity = vapaee::utils::asset_multiply(part, balance);
                        remaining -= quantity;
                        sum += part;
                        if (quantity.amount == 0) continue;

                        PRINT("pay_to_target[", std::to_string(i), "]:", target.c_str(), " - part(", part.to_string(), ") q(",quantity.to_string(), ")\n");
                        pay_to_target(quantity, target);
                        sub_payhub_balance(payhub_id, quantity);
                    }

                    asset one = asset(ipow(10, sum.symbol.precision()), sum.symbol);
                    check(one.amount == sum.amount,
                        create_error_asset2(
                            "ERR-DPM-02: the payhubs entry for the id was not found. (one, sum): ",
                            one,
                            sum
                        ).c_str()
                    );
                }

                check(remaining.amount == 0, 
                    create_error_asset2(
                        "ERR-DPM-03: the payhubs entry for the id was not found. (remaining, balance):",
                        remaining,
                        balance
                    ).c_str()
                );
            }

            void move_all_pockets_for_payhub(uint64_t payhub_id) {
                PRINT("vapaee::pay::hub::move_all_pockets_for_payhub()\n");
                PRINT(" payhub_id: ", std::to_string((long)payhub_id), "\n");
                name ram_payer = get_self();
                payhubs_table payhub;
                get_payhub_for_id(false, payhub_id, payhub, ram_payer, "ERR-MAPFP-01");
                
                paypockets funds_tbl( get_self(), get_self().value );
                auto index = funds_tbl.get_index<name("payhub")>();
                
                for (
                    auto ptr = index.find( payhub_id );
                    ptr != index.end();
                    ptr++
                ) {
                    move_pocket(payhub_id, ptr->balance.symbol.code());
                }
            }

            void move_any_pockets(int max) {
                PRINT("vapaee::pay::hub::move_all_pockets_for_payhub()\n");
                PRINT(" max: ", std::to_string(max), "\n");

                paypockets funds_tbl( get_self(), get_self().value );
                auto index = funds_tbl.get_index<name("oldest")>();
                int counter = max;
                
                for (auto itr = index.begin(); itr != index.end() && counter>0; counter--, itr++) {
                    PRINT(itr->to_string()," ",itr->balance.to_string()," in(", std::to_string((unsigned long)itr->last_in()), ") out(", std::to_string((unsigned long)itr->last_out()), ") oldest(", std::to_string((unsigned long)itr->by_oldest()), ") \n");
                    if (itr->balance.amount > 0) {
                        move_pocket(itr->payhub, itr->balance.symbol.code());
                    }
                }
            }

            void action_movepocket(const string& target, const name& signer) {
                PRINT("vapaee::pay::hub::action_movepocket()\n");
                PRINT(" target: ", target.c_str(), "\n");
                PRINT(" signer: ", signer.to_string(), "\n");

                require_auth(signer);
                name ram_payer = signer;

                // payhub 12356     --> wants to move all pockets for PayHub 
                // pocket 12356 CNT --> wants to move just the CNT pocket for PayHub
                // any 4            --> wants to move any four pockets
                
                vector<string> parts = split(target, " ");
                if (parts.size() >= 2) {
                    if (parts[0] == string("payhub")) {
                        uint64_t payhub_id = vapaee::utils::check_integer_from_string(parts[1]);
                        move_all_pockets_for_payhub(payhub_id);
                    }

                    if (parts[0] == string("pocket")) {
                        uint64_t payhub_id = vapaee::utils::check_integer_from_string(parts[1]);
                        symbol_code token = vapaee::utils::check_symbol_code_from_string(parts[2]);
                        move_pocket(payhub_id, token);                        
                    }

                    if (parts[0] == string("any")) {
                        int max = vapaee::utils::check_integer_from_string(parts[1]);
                        move_any_pockets(max);
                    }
                } else {
                    check(false,
                        create_error_string2("ERR-AMP-01: unrecognized target. (allowed, received)",
                        string("payhub <number>|pocket <number> <token>|any <number>"),
                        target
                    ).c_str());
                }
            }

            // handler ------
            void handle_payhub_payment(const asset& quantity, const string& target, const string& original_memo) {
                PRINT("vapaee::pay::hub::handle_payhub_payment()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" target: ", target.c_str(), "\n");
                PRINT(" original_memo: ", original_memo.c_str(), "\n");

                payhub_target pay_target;
                payhubs_table payhub;
                string generic_error = create_error_string1(
                    "ERROR on vapaeepayhub payment handler. You sent a transfer with memo 'pay <PayHub-id>' but that PayHub was not found. Ony id number or registered alias allowed. Got: ",
                    target
                );
                bool found;
                switch(parse_payhub_target(target, pay_target)) {
                    case TARGET_PAYHUB: {}
                    case TARGET_NAME: {
                        found = get_payhub_for_id(false, pay_target.payhub, payhub, get_self(), NULL);
                        check(found, string(string("ERR-HPHP-01: ") + generic_error).c_str());

                        // add to pocket balance
                        if (does_pocket_exist(pay_target.payhub, quantity.symbol.code())) {
                            add_payhub_balance(pay_target.payhub, quantity);
                        } else {
                            symbol_code token = payhub.main_pocket;
                            name receiver = get_self();

                            string swap_memo = string("pay ") + target + " | " + original_memo;
                            vapaee::pool::util::send_swap(quantity, token, receiver, swap_memo);
                        }
                        
                        break;
                    }
                    case TARGET_ACCOUNT: {
                        name contract = vapaee::dex::utils::get_contract_for_token(quantity.symbol.code());
                        vapaee::token::utils::send_transfer_tokens(get_self(), pay_target.account, quantity, original_memo, contract);
                        break;
                    }

                    default: {
                        check(false, string(string("ERR-HPHP-02: ") + generic_error).c_str());
                    }
                };
            }
        };     
    };
};
