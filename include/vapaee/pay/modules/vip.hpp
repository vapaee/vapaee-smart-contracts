#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/tables.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/pay/modules/rex.hpp>



namespace vapaee {
    namespace pay {
        namespace vip {

            inline name get_self() {
                return vapaee::pay::contract;
            }

            bool extract_pool_id(const string& target, pool_id& pool) {
                vector<string> parts = split(target, "-");
                if (parts.size() != 2) return false;
                // check(parts.size() > 0, create_error_string1("Error", target).c_str());
                
                name id;
                if (!vapaee::utils::get_name_from_string(parts[1], id)) return false;

                symbol_code token;
                if (!vapaee::utils::get_symbol_code_from_string(parts[0], token)) return false;

                pool.id = id;
                pool.token = token;
                return true;
            }

            bool get_alias (
                bool create,
                names_table& vip_name,
                const name& ram_payer,
                const char * error_code
            ) {
                // vapaee::pay::vip::get_alias
                names alias_tb(get_self(), get_self().value);
                auto index = alias_tb.get_index<name("alias")>();
                auto ptr = index.find(vapaee::utils::hash(vip_name.alias));

                if (ptr == index.end() && !create)  {
                    if (error_code) {
                        check(false, 
                            create_error_string1(
                                string(string(error_code) + ": the entry for the alias was not found. (alias): ").c_str(),
                                vip_name.alias
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (ptr == index.end() && create) {
                    alias_tb.emplace(ram_payer, [&](auto &a){ 
                        a.id         = alias_tb.available_primary_key();
                        a.owner      = vip_name.owner;
                        a.alias      = vip_name.alias;
                    });

                    return true;
                };

                if (ptr != index.end() && create) {
                    index.modify(ptr, ram_payer, [&](auto &a){
                        a.owner      = vip_name.owner;
                        a.alias      = vip_name.alias;
                    });

                    return true;
                };
                
                vip_name.id         = ptr->id;
                vip_name.owner      = ptr->owner;
                vip_name.alias      = ptr->alias;

                return true;
            }

            // owner buys a new alias
            void action_newname(const name& owner, const string& alias) {
                PRINT("vapaee::pay::vip::action_newname()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" alias: ", alias.c_str(), "\n");
                
                // For now these name are privated and not accesible for general public
                // until we figure out how to monetize the vip-name acquisition
                require_auth(get_self());

                // check it is not registered yet
                names_table vip_name;
                name ram_payer = owner;
                bool exists = vapaee::pay::vip::get_alias(false, vip_name, ram_payer, NULL);
                check(!exists, create_error_name1("ERR-ANN-01: name already registered by:", vip_name.owner));

                // Check it is not an account name
                name account;
                int err = vapaee::utils::get_name_from_string(alias, account);
                check(err != TYPERR_NO_ERROR, create_error_name1("ERR-ANN-02: alias can not be a potencial account name:", vip_name.owner));

                // check it is not a number
                bool it_is = vapaee::utils::is_natural(alias);
                check(!it_is, create_error_string1("ERR-ANN-03: alias can not be a number:", alias));

                // check it is not a potencial staking pool
                pool_id pool;
                bool fail = vapaee::pay::vip::extract_pool_id(alias, pool);
                check(fail, create_error_string1("ERR-ANN-04: alias can not be a potencial staking pool:", alias));

                vip_name.owner = owner;
                vip_name.alias = alias;
                vapaee::pay::vip::get_alias(true, vip_name, ram_payer, "ERR-ANN-05");
                     
            }


        };     
    };
};
