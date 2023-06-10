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
    std::string current_version = std::string("0.0.0");

    namespace bgbox {
        static const name contract = eosio::name("boardgamebox");
    };
    namespace pay {
        static const name contract = eosio::name("vapaeepayhub");
        static const symbol_code REX_TKN_CODE    = eosio::symbol_code("REX");
    };
    namespace cnt {
        static const name contract = eosio::name("cardsntokens");
    };
    namespace cat {
        static const name contract = eosio::name("categorizers");
    };
    namespace local {
        static const name contract = eosio::name("localstrings");
    };
    namespace style {
        static const name contract = eosio::name("vapaeestyles");
    };
    namespace token {
        static const name contract = eosio::name("vapaeetokens");
    };
    namespace dex {
        static const name contract = eosio::name("telosmaindex");
        uint8_t dex_internal_precision  = 8;
    };
    namespace book {
        static const name contract = eosio::name("telosbookdex");
    };
    namespace pool {
        static const name contract = eosio::name("telospooldex");
    };
    namespace atwitter {
        static const name    contract       = eosio::name("acorntwitter");
        static const name    acorn_contract = eosio::name("acornaccount");
        static const symbol  acorn_symbol   = symbol(symbol_code("ACORN"),4);
    };
    namespace wrap {
        static const name contract = eosio::name("teloswrapper");
        static const name rexcontract = eosio::name("eosio");

        static const symbol_code      TLOSW_TKN_CODE          = eosio::symbol_code("REX");
        static const symbol           TLOSW_TKN_SYMBOL        = eosio::symbol(vapaee::wrap::TLOSW_TKN_CODE, 4);
        static const symbol_code      TLOSV_TKN_CODE          = eosio::symbol_code("DEX");
        static const symbol           TLOSV_TKN_SYMBOL        = eosio::symbol(vapaee::wrap::TLOSV_TKN_CODE, 4);
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

using namespace vapaee::bgbox;
using namespace vapaee::pay;
using namespace vapaee::cnt;
using namespace vapaee::cat;
using namespace vapaee::local;
using namespace vapaee::style;
using namespace vapaee::token;
using namespace vapaee::dex;
using namespace vapaee::book;
using namespace vapaee::pool;
using namespace vapaee::atwitter;
using namespace vapaee::wrap;
using namespace vapaee::utils;

// hepper macros 

/*
#define MAINTENANCE(...) check(has_auth(_self), ">>>>>> This contract is in MAINTENANCE. Please, try later again. <<<<<<");
/*/
#define MAINTENANCE(...)
//*/

/*/
#define PRINT(...) print(__VA_ARGS__)
/*/
#define PRINT(...)
//*/

#define AUX_DEBUG_CODE(...)



#define MAX_VALUE 0xFFFFFFFFFFFFFFFF

// INVOICE_SYSTEM_FEE_PERCENT indicates de percentage of of the fee that a provider (like Koinonos) charges for each invoice
// If the proveder charges a fee of 0.1% for each invoice, then the fee is 0.1% * INVOICE_SYSTEM_FEE_PERCENT
#define INVOICE_SYSTEM_FEE_PERCENT 0.1


#define INVOICE_SYSTEM_FEE_PAYHUB_ALIAS string("vapaee")



#include <vapaee/base/utils.hpp>