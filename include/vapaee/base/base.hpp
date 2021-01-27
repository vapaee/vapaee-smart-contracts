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

// defining namespaces 
using namespace eosio;

namespace vapaee {
    namespace bgbox {
        static name contract = eosio::name("boardgamebox");
    };
    namespace cnt {
        static name contract = eosio::name("cardsntokens");
    };
    namespace cat {
        static name contract = eosio::name("categorizers");
    };
    namespace str {
        static name contract = eosio::name("localstrings");
    };
    namespace tprofile {
        static name contract = eosio::name("telosprofile");
    };
    namespace author {
        static name contract = eosio::name("vapaeeauthor");
    };
    namespace style {
        static name contract = eosio::name("vapaeestyles");
    };
    namespace token {
        static name contract = eosio::name("vapaeetokens");
    };
    namespace ttracker {
        static name contract = eosio::name("eventtracker");
    };
    namespace dex {
        static name contract = eosio::name("telosmaindex");
        uint8_t internal_precision  = 8;
    };
    namespace utils {
        static symbol_code      SYS_TKN_CODE      = eosio::symbol_code("TLOS");
        static name             SYS_TKN_CONTRACT  = eosio::name("eosio.token");

    }
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
using namespace utils;

#include <vapaee/base/utils.hpp>

// hepper macros 

/*
#define MAINTENANCE(...) check(has_auth(_self), ">>>>>> This contract is in MAINTENANCE. Please, try later again. <<<<<<");
/*/
#define MAINTENANCE(...)
//*/

/*
#define PRINT(...) print(__VA_ARGS__)
#define AUX_DEBUG_CODE(...) __VA_ARGS__
/*/
#define PRINT(...)
#define AUX_DEBUG_CODE(...)
//*/
