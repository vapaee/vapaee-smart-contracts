struct pool_id {
    name label;                         // Each pool identifies with the text: "<symcode>-<label>". i.e: "CNT-main", "VPE-canvas", "VPE-dex"
    symbol_code token;                  // Symbol code of the token to stake (equals skope value)
    std::string to_string() const {
        return token.to_string() + "-" + label.to_string();
    };
    uint128_t pack() {
        return vapaee::utils::pack( token.raw(), label.value );
    };    
};
