#pragma once
#include <vapaee/base/modules/global.hpp>

namespace vapaee {

    CONTRACT pruebavapaee : public eosio::contract {
        
        private:

            #include <vapaee/base/tables.all.hpp>

        public:
            using contract::contract;

            string get_version() { return string("0.0.1"); }

            pruebavapaee(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver;  vapaee::current_version = get_version();  }


            // Global module
            ACTION init() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::init() ------------------\n");
                vapaee::base::global::action_init();
            };
            
            // Test function
            ACTION test(name a) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::test() ------------------\n");
            };

    }; // contract class

}; // vapaee namespace

// To fix the following error when calling an action:
//   Error 3050004: eosio_assert_code assertion failure
//   Error Details:
//   assertion failure with error code: 8000000000000000000
// uncomment the following line
// EOSIO_DISPATCH(pruebavapaee, (init)(test))