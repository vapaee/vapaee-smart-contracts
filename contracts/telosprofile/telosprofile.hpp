#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/dispatcher.hpp>
#include <vapaee/profile/modules/core.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace profile;

CONTRACT telosprofile : public eosio::contract {
    
    private:

#include <vapaee/profile/tables.all.hpp>

    public:
        using contract::contract;

    public:

        // Client Module ---------------------------------------------------------------------
        
        ACTION test (name contract, const slug & alias, uint8_t precision) {
            MAINTENANCE();
            PRINT("\nACTION telosprofile.test() ------------------\n");
            vapaee::profile::core::action_test(contract, alias, precision);
        };
    
        // habdler for tokenaccount::transfer
        HANDLER htransfer(name from, name to, asset quantity, string  memo ) {
            PRINT("\nHANDLER telosprofile.htransfer() ------------------\n");
            
            // skip handling outcoming transfers from this contract to outside
            if (from == get_self()) {
                PRINT(from.to_string(), " to ", to.to_string(), ": ", quantity.to_string(), " eosio::telosprofile::handler_transfer() skip...\n");
                return;
            }
            
            // handler transfer...
        }
                
        AUX_DEBUG_CODE (

            ACTION hotfix (int max, name scope, asset q) {
                PRINT("\nACTION telosprofile.hotfix() ------------------\n");
                // do hot fix
            };

        )
};

}; // eosio namespace