#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/dispatcher.spp>
#include <vapaee/book/modules/deposit.hpp>
#include <vapaee/book/modules/exchange.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace book;

    CONTRACT telosbookdex : public eosio::contract {
        
        private:

            #include <vapaee/book/tables.all.hpp>

        public:
            using contract::contract;

            telosbookdex(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }
                            
            // Exchange module
            ACTION cancel(
                name owner,
                name type,
                const symbol_code & commodity,
                const symbol_code & currency,
                const std::vector<uint64_t> & orders
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosbookdex.cancel() ------------------\n");
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
                PRINT("\nACTION telosbookdex.order() ------------------\n");
                vapaee::book::exchange::action_order(owner, type, total, price, client);
            };

            // Deposit Module
            ACTION withdraw(
                name owner,
                const asset & quantity,
                uint64_t client
            ) {
                MAINTENANCE();
                PRINT("\nACTION telosbookdex.withdraw() ------------------\n");
                vapaee::book::deposit::action_withdraw(owner, quantity, client);
            };
        
            // handler for *::transfer
            HANDLER htransfer(
                name from,
                name to,
                asset quantity,
                string  memo
            ) {
                // MAINTENANCE();
                PRINT("\nHANDLER telosbookdex.htransfer() ------------------\n");
                
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
