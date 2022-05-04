#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/dex/tables.hpp>


/*

*****************
** tabla: rexpool -> https://github.com/telosnetwork/telos.contracts/blob/master/contracts/eosio.system/include/eosio.system/eosio.system.hpp#L417
TABLE rex_pool {
    uint8_t    version = 0;
    asset      total_lent;
    asset      total_unlent;
    asset      total_rent;
    asset      total_lendable;
    asset      total_rex;
    asset      namebid_proceeds;
    uint64_t   loan_num = 0;
    uint64_t primary_key()const { return 0; }
};
$ telosmain get table eosio eosio rexpool
{
    "version": 0,
    "total_lent": "22200.0298 TLOS",
    "total_unlent": "150799817.5834 TLOS",
    "total_rent": "98441.7975 TLOS",
    "total_lendable": "150822017.6132 TLOS",
    "total_rex": "1004792855427.9971 REX",
    "namebid_proceeds": "0.0000 TLOS",
    "loan_num": 460
}

*****************
** tabla: rexbal -> https://github.com/telosnetwork/telos.contracts/blob/master/contracts/eosio.system/include/eosio.system/eosio.system.hpp#L474
TABLE rex_balance {
    uint8_t version = 0;
    name    owner;
    asset   vote_stake;
    asset   rex_balance;
    int64_t matured_rex = 0;
    std::deque<std::pair<time_point_sec, int64_t>> rex_maturities;
    uint64_t primary_key()const { return owner.value; }
};
$ telosmain get table eosio eosio rexbal --lower viterbotelos --upper viterbotelos
{
    "version": 0,
    "owner": "viterbotelos",
    "vote_stake": "100374.1605 TLOS",
    "rex_balance": "669371444.0443 REX",
    "matured_rex": 4692,
    "rex_maturities": [{
        "first": "2022-01-09T00:00:00",
        "second": "6816510806"
    },{
        "first": "2106-02-07T06:28:15",
        "second": "6686897924945"
    }]
}

*/

namespace vapaee {
    namespace wrap {
        namespace rex {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            inline name get_rex() {
                return vapaee::wrap::rexcontract;
            }

            // REX tables structures ---------
            // structure of table eosio.rexfund
            TABLE rex_fund {
                uint8_t version = 0;
                name    owner;
                asset   balance;

                uint64_t primary_key()const { return owner.value; }
            };

            typedef eosio::multi_index< "rexfund"_n, rex_fund > rexfund_table;

            // structure of table eosio.rexpool
            TABLE rex_pool {
                uint8_t    version = 0;
                asset      total_lent;
                asset      total_unlent;
                asset      total_rent;
                asset      total_lendable;
                asset      total_rex;
                asset      namebid_proceeds;
                uint64_t   loan_num = 0;
                uint64_t primary_key() const { return 0; }
            };

            typedef eosio::multi_index< "rexpool"_n, rex_pool > rexpool_table;

            // structure of table eosio.rexbal
            TABLE rex_balance {
                uint8_t version = 0;
                name    owner;
                asset   vote_stake;
                asset   rex_balance;
                int64_t matured_rex = 0;
                std::deque<std::pair<time_point_sec, int64_t>> rex_maturities;
                uint64_t primary_key()const { return owner.value; }
            };
            
            typedef eosio::multi_index< "rexbal"_n, rex_balance > rexbal_table;

            // --------------------------------

            rex_balance get_rex_balance(name owner) {
                PRINT("vapaee::wrap::rex::get_rex_balance()\n");
                rexbal_table table(get_rex(), get_rex().value);
                auto itr = table.find(owner.value);
                check(itr != table.end(), create_error_name1(ERROR_GRB_1, owner).c_str());
                return *itr;
            }

            rex_pool get_rex_pool() {
                PRINT("vapaee::wrap::rex::get_rex_pool()\n");
                rexpool_table table(get_rex(), get_rex().value);
                auto itr = table.begin();
                check(itr != table.end(), create_error_name1(ERROR_GRP_1, get_rex()).c_str());
                return *itr;
            }

            rex_fund get_rex_fund(name owner) {
                PRINT("vapaee::wrap::rex::get_rex_fund()\n");
                rexfund_table table(get_rex(), get_rex().value);
                auto itr = table.find(owner.value);
                check(itr != table.end(), create_error_name1(ERROR_GRF_1, owner).c_str());
                return *itr;
            }

            // --------------------------------

            asset get_total_assets_in_tlos() {
                PRINT("vapaee::wrap::rex::get_total_assets_in_tlos()\n");
                asset total_assets = asset(0, vapaee::utils::SYS_TKN_SYMBOL);
                
                rex_balance balance = vapaee::wrap::rex::get_rex_balance(get_self());
                
                rex_pool pool = vapaee::wrap::rex::get_rex_pool();
                const int64_t total_rex      = pool.total_rex.amount;
                const int64_t total_lendable = pool.total_lendable.amount;
                const int64_t rex_balance    = balance.rex_balance.amount;

                total_assets.amount = ( uint128_t(rex_balance) * total_lendable ) / total_rex;

                return total_assets;
            }

            asset get_matured_rex() {
                // PRINT("vapaee::wrap::rex::get_matured_rex()\n");
                return asset(vapaee::wrap::rex::get_rex_balance(get_self()).matured_rex, vapaee::utils::REX_TKN_SYMBOL);
            }


            asset transform_tlos_to_rex(asset quantity) {
                PRINT("vapaee::wrap::rex::transform_tlos_to_rex()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                rex_pool pool = vapaee::wrap::rex::get_rex_pool();
                const int64_t total_rex       = pool.total_rex.amount;
                const int64_t total_lendable  = pool.total_lendable.amount;
                const int64_t quantity_amount = quantity.amount;

                const int64_t rex_amount      = uint128_t(quantity_amount) * total_rex / total_lendable;

                asset rex = asset (rex_amount, pool.total_rex.symbol);

                PRINT(" ->rex: ", rex.to_string(), "\n");

                return rex;
            }

            asset transform_rex_to_tlos(asset rex) {
                PRINT("vapaee::wrap::rex::transform_tlos_to_rex()\n");
                PRINT(" rex: ", rex.to_string(), "\n");

                rex_pool pool = vapaee::wrap::rex::get_rex_pool();
                const int64_t total_rex       = pool.total_rex.amount;
                const int64_t total_lendable  = pool.total_lendable.amount;
                const int64_t rex_amount      = rex.amount;

                const int64_t quantity_amount = uint128_t(rex_amount) * total_lendable / total_rex;

                asset quantity = asset (quantity_amount, pool.total_lendable.symbol);

                PRINT(" ->quantity: ", rex.to_string(), "\n");

                return quantity;
            }

            asset get_rex_price() {
                PRINT("vapaee::wrap::rex::transform_tlos_to_rex()\n");

                rex_pool pool = vapaee::wrap::rex::get_rex_pool();

                asset rex_extended = vapaee::dex::utils::aux_extend_asset(asset(10000, vapaee::utils::REX_TKN_SYMBOL));
                asset total_rex_extended = vapaee::dex::utils::aux_extend_asset(pool.total_rex);
                asset total_lendable_extended = vapaee::dex::utils::aux_extend_asset(pool.total_lendable);

                const int64_t total_rex       = total_rex_extended.amount;
                const int64_t total_lendable  = total_lendable_extended.amount;
                const int64_t rex_amount      = rex_extended.amount;

                const int64_t quantity_amount = uint128_t(rex_amount) * total_lendable / total_rex;

                asset quantity = asset (quantity_amount, total_lendable_extended.symbol);

                PRINT(" -> get_rex_price: ", quantity.to_string(), "\n");

                return quantity;
            }            

            asset get_max_liquidity() {
                PRINT("vapaee::wrap::rex::get_max_liquidity()\n");
                rex_pool pool = vapaee::wrap::rex::get_rex_pool();
                const int64_t total_lendable = pool.total_lendable.amount;
                const int64_t total_rex      = pool.total_rex.amount;

                asset max_liquidity = asset(total_lendable, pool.total_lendable.symbol);

                PRINT(" ->max_liquidity: ", max_liquidity.to_string(), "\n");

                // TODO: get_max_liquidity
                return max_liquidity;
            }

            asset deposit_tlos_to_rex(asset quantity) {
                PRINT("vapaee::wrap::rex::deposit_tlos_to_rex()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // check if quantity is in TLOS
                check(quantity.symbol == vapaee::utils::SYS_TKN_SYMBOL, create_error_asset1(ERROR_DTTR_1, quantity).c_str());

                // calculate how many REX we can get
                asset rex = transform_tlos_to_rex(quantity);

                // cleos push action eosio deposit '["alice", "100.0000 TLOS"]' -p alice@active
          
                action(
                    permission_level{get_self(), name("active")},
                    get_rex(),
                    name("deposit"),
                    std::make_tuple(get_self(), quantity)
                ).send();

                // cleos push action eosio buyrex '["alice", "100.0000 TLOS"]' -p alice@active
                action(
                    permission_level{get_self(), name("active")},
                    get_rex(),
                    name("buyrex"),
                    std::make_tuple(get_self(), quantity)
                ).send();

                return rex;
            }

            

            /**
            * @dev Withdraws REX tokens from the REX pool.
            * @param rex The amount of REX tokens to withdraw.
            * @return The amount of TLOS tokens withdrawn.
            */
            asset withdraw_REX(asset rex) {
                PRINT("vapaee::wrap::rex::withdraw_REX()\n");
                PRINT(" quantity: ", rex.to_string(), "\n");

                // check if quantity is in TLOS
                check(rex.symbol == vapaee::utils::REX_TKN_SYMBOL, create_error_asset1(ERROR_WDR_1, rex).c_str());
                asset quantity = transform_rex_to_tlos(rex);
                // cleos push action eosio sellrex '["viterbotelos", "39873772.1234 REX"]' -p viterbotelos@active
                action(
                    permission_level{get_self(), name("active")},
                    get_rex(),
                    name("sellrex"),
                    std::make_tuple(get_self(), rex)
                ).send();

                // cleos push action eosio withdraw '["viterbotelos", "6013.0876 TLOS"]' -p viterbotelos@active
                action(
                    permission_level{get_self(), name("active")},
                    get_rex(),
                    name("withdraw"),
                    std::make_tuple(get_self(), quantity)
                ).send();

                return quantity;
            }


            asset withdraw_TLOS(asset quantity) {
                PRINT("vapaee::wrap::rex::withdraw_TLOS()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // check if quantity is in TLOS
                check(quantity.symbol == vapaee::utils::SYS_TKN_SYMBOL, create_error_asset1(ERROR_WDT_1, quantity).c_str());

                asset rex = transform_tlos_to_rex(quantity);

                action(
                    permission_level{get_self(), name("active")},
                    get_rex(),
                    name("sellrex"),
                    std::make_tuple(get_self(), rex)
                ).send();

                // cleos push action eosio withdraw '["viterbotelos", "6013.0876 TLOS"]' -p viterbotelos@active
                action(
                    permission_level{get_self(), name("active")},
                    get_rex(),
                    name("withdraw"),
                    std::make_tuple(get_self(), quantity)
                ).send();

                return rex;
            }

        };     
    };
};
