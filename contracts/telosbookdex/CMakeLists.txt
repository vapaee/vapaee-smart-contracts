cmake_minimum_required(VERSION 3.5)
project(telos_dex VERSION 0.1.0)
set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)
include(EosioWasmToolchain)

add_contract(telosbookdex telosbookdex telosbookdex.cpp)

file(GLOB custom_includes ${CUSTOM_INCLUDES_DIR}/*)
target_include_directories(telosbookdex PUBLIC ${custom_includes})
