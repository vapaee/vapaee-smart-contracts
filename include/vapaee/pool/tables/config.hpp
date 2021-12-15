TABLE configtable {
    asset conversion_fee;
} config_row;

typedef singleton<"config"_n, configtable> pool_config;
