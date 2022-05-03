#pragma once

#include <vapaee/pool/utils.hpp>
#include <vapaee/pool/modules/handler.hpp>
#include <vapaee/pool/modules/liquidity.hpp>
#include <vapaee/pool/modules/swap.hpp>

namespace vapaee {

    namespace pool {

        CONTRACT telospooldex : public eosio::contract {

            private:
                #include <vapaee/pool/tables.all.hpp>

            public:
                using contract::contract;

                telospooldex(name receiver, name code, datastream<const char*> ds) :
                    contract(receiver, code, ds)
                    { vapaee::current_contract = receiver; }

                ACTION cancelfund(name funder, uint64_t market_id) {
                    MAINTENANCE();
                    PRINT("\nACTION telospooldex.cancelfund() ------------------\n");
                    vapaee::pool::liquidity::action_cancel_fund(funder, market_id);
                }

                ACTION takepart(name funder, uint64_t market_id, asset score) {
                    MAINTENANCE();
                    PRINT("\nACTION telospooldex.takepart() ------------------\n");
                    vapaee::pool::liquidity::action_withdraw_participation(funder, market_id, score);
                }

                [[eosio::on_notify("*::transfer")]]
                void handle_transfer(
                    name from,
                    name to,
                    asset quantity,
                    string memo
                ) {
                    MAINTENANCE();
                    PRINT("\nHANDLER telospooldex.htransfer() ------------------\n");
                    
                    // skip handling transfers from this contract to outside
                    if (from == vapaee::dex::contract)
                        return;
                    
                    MAINTENANCE();
                    vapaee::pool::handler::handle_pool_transfer(
                        from, to, quantity, memo, get_first_receiver());
                }

        };  // contract class

    };  // namespace pool

};  // namespace vapaee
