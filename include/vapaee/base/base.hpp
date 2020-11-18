#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/symbol.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/print.hpp>
#include <eosio/crypto.hpp>

#include <ctype.h>
#include <stdlib.h>

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
    namespace utils {}
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
using namespace utils;

#include <vapaee/base/slug.hpp>
#include <vapaee/base/slug_asset.hpp>
#include <vapaee/base/slug_symbol.hpp>
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