#!/bin/bash



if [[ ! -f ./contracts/telos.decide/build/contracts/decide/decide.wasm ||
      ! -f ./contracts/telos.decide/build/contracts/watcher/watcher.wasm ||
      ! -f ./contracts/eosio.token/build/eosio.token.wasm ||
      ! -f ./contracts/eventhandler/build/eventhandler.wasm ||
      ! -f ./contracts/eventtracker/build/eventtracker.wasm ||
      ! -f ./contracts/telosbookdex/build/telosbookdex.wasm ||
      ! -f ./contracts/telosprofile/build/telosprofile.wasm ||
      ! -f ./contracts/testcontract/build/testcontract.wasm ]]; then
    echo "Some of the WASMs are not present."
    echo "Pytest will be launched to build contracts and test them."
    pytest
fi

echo "Building docker image..."
sleep 1
docker build --tag vtestnet:eosio-inter     --build-arg USER_ID=$(id -u)     --build-arg GROUP_ID=$(id -g) docker/vtestnet

echo "Running container..."
sleep 1
docker run -it --mount type=bind,source="$(pwd)",target=/vapaee-smart-contracts -p 8888:8888 vtestnet:eosio-inter bash

