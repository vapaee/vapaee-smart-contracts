// -- state --
// All the configuration and temporal state 

// scope: singleton
// row: represents a singleton state

TABLE state {
    asset maker_fee;
    asset taker_fee;
    asset swap_fee;

    int hprune;  // amount of days in which a HISTORY entry must be considered expired and must be removed 
    int kprune;  // amount of days in which a HISTORY BLOCKS entry must be considered expired and must be removed 
    int bprune;  // amount of maximun finished BALLOT entries allowed.
    int eprune;  // amount of days in which a EVENT entry must be considered expired and must be removed 
    int pprune;  // amount of weeks in which a POINTS entry must be considered expired and must be removed 
    float approvalmin; // minimum participation percentage for ballot approval
    asset regcost; // token registration cost (in TLOS)
    
    float maker_pts_reward; // trading points reward multiplier
    float maker_exp_reward; // trading experience reward multiplier
    float taker_pts_reward; // trading points reward multiplier
    float taker_exp_reward; // trading experience reward multiplier

    float maint_reward_delmarkets_exp;
    float maint_reward_history_exp;
    float maint_reward_events_exp;
    float maint_reward_points_exp;
    float maint_reward_ballots_exp;

    float maint_reward_delmarkets_pts;
    float maint_reward_history_pts;
    float maint_reward_events_pts;
    float maint_reward_points_pts;
    float maint_reward_ballots_pts;

    // This property is here because we can't reutilize ids for markets. If a token gets blacklisted,
    // all the markets involving that token will be slowly cleaned up by the maintenance process.
    // If we create a new market reusing the id of the deletedone, we may end up "cleaning up" the wrong market
    uint64_t next_market; // id of the next market
    AUX_DEBUG_CODE(uint32_t time_offset;)
};

typedef singleton<name("state"), state> global_state_singleton;
