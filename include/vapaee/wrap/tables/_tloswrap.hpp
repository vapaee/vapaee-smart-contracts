#pragma once

struct tloswrap {
    asset tlosv;                  // amount in TLOSV
    asset tlosw;                  // amount in TLOSW

/*
    // in the constructor it accepts both TLOSV and TLOSW
    // example: tloswrap(asset(100, "TLOSV"), asset(200, "TLOSW"));
    tloswrap(asset tlosv, asset tlosw) {
        this->tlosv = tlosv;
        this->tlosw = tlosw;
    }

    // but also we need the default constructor with no parameters to initialize the struct with 0 TLOSV and 0 TLOSW
    tloswrap() {
        this->tlosv = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);
        this->tlosw = asset(0, vapaee::wrap::TLOSW_TKN_SYMBOL);
    }
    */
    
    asset total() const { return asset(tlosv.amount+tlosw.amount, vapaee::utils::SYS_TKN_SYMBOL); }
    string to_string() { return tlosv.to_string() + " " + tlosw.to_string(); }
};

