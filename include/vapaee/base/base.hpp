#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/symbol.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/print.hpp>
#include <eosio/crypto.hpp>
#include <eosio/time.hpp>

#include <math.h>
#include <ctype.h>
#include <stdlib.h>

#include <tuple>
#include <algorithm>

// defining namespaces 
using namespace eosio;

namespace vapaee {
    name current_contract = eosio::name("vapaee");
    
    namespace bgbox {
        static const name contract = eosio::name("boardgamebox");
    };
    namespace pay {
        static const name contract = eosio::name("vapaeepayhub");
    };
    namespace cnt {
        static const name contract = eosio::name("cardsntokens");
    };
    namespace cat {
        static const name contract = eosio::name("categorizers");
    };
    namespace str {
        static const name contract = eosio::name("localstrings");
    };
    namespace tprofile {
        static const name contract = eosio::name("telosprofile");
    };
    namespace author {
        static const name contract = eosio::name("vapaeeauthor");
    };
    namespace style {
        static const name contract = eosio::name("vapaeestyles");
    };
    namespace token {
        static const name contract = eosio::name("vapaeetokens");
    };
    namespace ttracker {
        static const name contract = eosio::name("eventtracker");
    };
    namespace dex {
        static const name contract = eosio::name("telosmaindex");
        uint8_t internal_precision  = 8;
    };
    namespace book {
        static const name contract = eosio::name("telosbookdex");
    };
    namespace pool {
        static const name contract = eosio::name("telospooldex");
    };
    namespace wrap {
        static const name contract = eosio::name("teloswrapper");
        static const name rexcontract = eosio::name("eosio");

        static const symbol_code      TLOSW_TKN_CODE          = eosio::symbol_code("REX");
        static const symbol           TLOSW_TKN_SYMBOL        = eosio::symbol(vapaee::wrap::TLOSW_TKN_CODE, 4);
        static const symbol_code      TLOSV_TKN_CODE          = eosio::symbol_code("DEX");
        static const symbol           TLOSV_TKN_SYMBOL        = eosio::symbol(vapaee::wrap::TLOSV_TKN_CODE, 4);
    };
    namespace echo {
        static const name             contract                = eosio::name("echocontract");
    };
    namespace nft {
        static const name contract = eosio::name("nftcontainer");
    };
    namespace utils {
        static const name             SYS_TKN_CONTRACT        = eosio::name("eosio.token");
    
        static const symbol_code      FEE_TKN_CODE            = eosio::symbol_code("FEE");
        static const symbol           FEE_TKN_SYMBOL          = eosio::symbol(vapaee::utils::FEE_TKN_CODE, 4);
        static const symbol_code      SYS_TKN_CODE            = eosio::symbol_code("TLOS");
        static const symbol           SYS_TKN_SYMBOL          = eosio::symbol(vapaee::utils::SYS_TKN_CODE, 4);
        static const symbol_code      REX_TKN_CODE            = eosio::symbol_code("REX");
        static const symbol           REX_TKN_SYMBOL          = eosio::symbol(vapaee::utils::REX_TKN_CODE, 4);

        static const name             OPENPOOL_PROTOCOL_NAME  = eosio::name("openpool.v1");
    };
};

using namespace vapaee;
using namespace bgbox;
using namespace cnt;
using namespace cat;
using namespace str;
using namespace tprofile;
using namespace author;
using namespace style;
using namespace token;
using namespace dex;
using namespace book;
using namespace pool;
using namespace utils;

#include <vapaee/base/utils.hpp>

// hepper macros 

/*
#define MAINTENANCE(...) check(has_auth(_self), ">>>>>> This contract is in MAINTENANCE. Please, try later again. <<<<<<");
/*/
#define MAINTENANCE(...)
//*/

//*/
#define PRINT(...) print(__VA_ARGS__)
#define AUX_DEBUG_CODE(...) __VA_ARGS__
/*/
#define PRINT(...)
#define AUX_DEBUG_CODE(...)
//*/
