#pragma once

#include <tuple>
#include <string>
#include <cstring>
#include <algorithm>

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/symbol.hpp>

using std::string;
using std::vector;
using std::strlen;
using std::strtok;
using std::replace;

using eosio::asset;


inline int64_t ipow(int64_t base, uint64_t exp) {
    if (exp == 0) return 1;

    int64_t result = base;
    for(int i = 0; i < (exp - 1); i++)
        result *= base;

    return result;
}


vector<string> split(const string &txt, const char * delim) {
    vector<string> tokens;
    string buffer(txt);
    char* token = strtok(&buffer[0], delim);
    while(token) {
        tokens.push_back(string(token, token + strlen(token)));
        token = strtok(NULL, delim);
    }
    return tokens;
}

int128_t divide(const asset &A, const asset &B) {
    asset accurate, inaccurate;

    if (A.symbol.precision() > B.symbol.precision()) {
        accurate = A;
        inaccurate = B;
    } else {
        accurate = B;
        inaccurate = A;
    }

    // augment precision of most inaccurate amount
    int dif = accurate.symbol.precision() - inaccurate.symbol.precision();

    int128_t _accurate_amount = accurate.amount;
    int128_t _fixed_amount = (int128_t)inaccurate.amount * ipow(10, dif);

    // perform operation and add extra precision destroyed by operation
    int128_t result = (
        _fixed_amount * ipow(10, accurate.symbol.precision())
    ) / _accurate_amount;

    // if function should return amount using
    // most accurate precision ret here

    // if function should return using B asset precision 
    dif = accurate.symbol.precision() - B.symbol.precision();

    if (dif == 0) // B was the most accurate
        return result;
    else
        return result / ipow(10, dif);
        // dif > 0, B had less precision than A, remove extra precision

}

asset asset_divide(const asset &A, const asset &B) {
    return asset(divide(A, B), B.symbol);
}

int128_t multiply(const asset &A, const asset &B) {
    asset accurate, inaccurate;

    if (A.symbol.precision() > B.symbol.precision()) {
        accurate = A;
        inaccurate = B;
    } else {
        accurate = B;
        inaccurate = A;
    }

    // augment precision of most inaccurate amount
    int dif = accurate.symbol.precision() - inaccurate.symbol.precision();

    int128_t _accurate_amount = accurate.amount;
    int128_t _fixed_amount = (int128_t)inaccurate.amount * ipow(10, dif);

    // perform operation and remove extra precision created by operation
    int128_t result = _fixed_amount * _accurate_amount;
    result /= ipow(10, accurate.symbol.precision());

    // if function should return amount using
    // most accurate precision ret here

    // if function should return using B asset precision 
    dif = accurate.symbol.precision() - B.symbol.precision();

    if (dif == 0) // B was the most accurate
        return result;
    else
        return result / ipow(10, dif);
        // dif > 0, B had less precision than A, remove extra precision

}

asset asset_multiply(const asset &A, const asset &B) {
    return asset(multiply(A, B), B.symbol);
}
