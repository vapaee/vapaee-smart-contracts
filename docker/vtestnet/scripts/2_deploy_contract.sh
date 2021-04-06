#!/bin/bash

HOME=${WORK_DIR}
#-------------------------------
INCLUDE_PATH=$HOME/include
force=false
NET=

# checking 'force' param
if [ "$1" == "force" ]; then
   force=true
fi

if [ "$2" == "force" ]; then
   force=true
fi

if [ "$1" == "prod" ]; then
   NET='--url https://telos.eos.barcelona'
fi

if [ "$1" == "test" ]; then
   NET='--url https://testnet.telos.caleos.io'
fi




echo "-------- 2_deploy_contract ---------"

echo "-------- telosbookdex ---------"
DEX_CONTRACT_DIR=$HOME/contracts/telosbookdex
DEX_WASM_FILE=build/telosbookdex.wasm
cd $DEX_CONTRACT_DIR
if [[ $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/client.hpp       -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/dao.hpp          -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/deposit.hpp      -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/error.hpp        -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/exchange.hpp     -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/experience.hpp   -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/global.hpp       -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/maintenance.hpp  -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/market.hpp       -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/record.hpp       -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/token.hpp        -nt $DEX_WASM_FILE ||
      $DEX_CONTRACT_DIR/../../include/vapaee/dex/modules/utils.hpp        -nt $DEX_WASM_FILE ||
      telosbookdex.cpp                                                    -nt $DEX_WASM_FILE ||
      telosbookdex.hpp                                                    -nt $DEX_WASM_FILE || 
      $force == true ]]; then
   mkdir build
   eosio-cpp -o $DEX_WASM_FILE telosbookdex.cpp --abigen -I "$INCLUDE_PATH"
fi
echo "DEX_WASM_FILE = $DEX_WASM_FILE"
echo "cleos $NET set contract telosbookdex $PWD/build telosbookdex.wasm telosbookdex.abi -p telosbookdex@active"
cleos $NET set contract telosbookdex $PWD/build telosbookdex.wasm telosbookdex.abi -p telosbookdex@active


EVENTHANDLER_DIR=$HOME/contracts/eventhandler
EVENT_WASM_FILE=build/eventhandler.wasm
if [ "$NET" == "" ]; then
    echo "-------- eventhandler ---------"
    cd $EVENTHANDLER_DIR
    pwd
    if [[ eventhandler.hpp -nt $EVENT_WASM_FILE || 
        $force == true ]]; then
        mkdir build
        eosio-cpp -o $EVENT_WASM_FILE eventhandler.cpp --abigen -I "$INCLUDE_PATH"
    fi
    echo "cleos $NET set contract eventhandler $PWD/build -p eventhandler@active"
    cleos $NET set contract eventhandler $PWD/build eventhandler.wasm eventhandler.abi -p eventhandler@active
fi


echo "-------- 2_deploy_contract finished ---------"