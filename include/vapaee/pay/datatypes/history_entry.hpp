
struct history_entry {
    time_point_sec date;             // date of the snapshot
    asset quantity;                  // 
    std::string to_string() const {
        return string("stakeconfigs [date:") + std::to_string((unsigned long)date.utc_seconds) + " quantity:" + quantity.to_string() + "]";
    };
};
