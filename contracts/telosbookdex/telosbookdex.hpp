#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/token/modules/utils.hpp>
#include <vapaee/book/modules/deposit.hpp>
#include <vapaee/book/modules/exchange.hpp>

namespace vapaee {
    using namespace book;

    CONTRACT telosbookdex : public eosio::contract {
        
        private:

            #include <vapaee/book/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>

        public:
            using contract::contract;

            string get_version() { return string("0.9.2"); } // telosbookdex-0.9.2

            telosbookdex(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver;  vapaee::current_version = get_version();  }


            // Global module
            ACTION init() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::init() ------------------\n");
                vapaee::base::global::action_init();
            };                
                            
            // Exchange module
            ACTION cancel(
                name owner,
                name type,
                const symbol_code & commodity,
                const symbol_code & currency,
                const std::vector<uint64_t> & orders
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::cancel() ------------------\n");
                vapaee::book::exchange::action_cancel(owner, type, commodity, currency, orders);
            };

            ACTION order(
                name owner,
                name type,
                const asset & total,
                const asset & price,
                uint64_t client
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::order() ------------------\n");
                vapaee::book::exchange::action_order(owner, type, total, price, client);
            };

            // Deposit Module
            ACTION withdraw(
                name owner,
                const asset & quantity,
                uint64_t client
            ) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::withdraw() ------------------\n");
                vapaee::book::deposit::action_withdraw(owner, quantity, client);
            };

            ACTION clear()  {
                PRINT("\nACTION ", vapaee::current_contract.to_string(), "::clear() ------------------\n");
                require_auth(get_self()); // Only the contract itself can call this

                // Iterate depusers and nested deposits + userorders
                depusers dus(get_self(), get_self().value);
                for (auto idu = dus.begin(); idu != dus.end(); ) {
                    uint64_t acct = idu->account.value;

                    // Clear deposits (scope = acct)
                    deposits deps(get_self(), acct);
                    for (auto id = deps.begin(); id != deps.end(); id = deps.erase(id));

                    // Clear userorders (scope = acct)
                    userorders uo(get_self(), acct);
                    for (auto iu = uo.begin(); iu != uo.end(); iu = uo.erase(iu));

                    idu = dus.erase(idu);
                }

                // Collect scopes for sellorders while Clear ordersummary
                std::vector<uint64_t> scopes;
                ordersummary os(get_self(), get_self().value);
                sellorders sdummy(get_self(), get_self().value);
                for (auto ios = os.begin(); ios != os.end(); ) {
                    uint64_t scope = ios->market;
                    scopes.push_back(scope);
                    
                    // Clear ordersummary
                    ios = os.erase(ios);
                }

                // Clear sellorders per market scope
                for (auto m : scopes) {
                    sellorders so(get_self(), m);
                    for (auto iso = so.begin(); iso != so.end(); iso = so.erase(iso));
                }
            }
            
            [[eosio::on_notify("*::transfer")]]
            void htransfer(
                name from,
                name to,
                asset quantity,
                string  memo
            ) {
                // MAINTENANCE();
                PRINT("\nHANDLER telosbookdex::htransfer() ------------------\n");
                
                // skip handling transfers from this contract to outside
                if (from == vapaee::book::contract)
                    return;
                
                if (memo == "deposit") {
                    MAINTENANCE();
                    vapaee::book::deposit::handle_book_transfer(
                        from, to, quantity, memo, get_first_receiver());
                }
            }

    }; // contract class

}; // vapaee namespace
