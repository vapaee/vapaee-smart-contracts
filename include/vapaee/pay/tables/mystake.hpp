// -- mystake --
// Each user can stake difefrent tokens. For each token, the user will have a global stake state gathering information from all pools of the same token along with the mystake credits configurations of the user.
// This table holds theglobal states of all the mystake tokens for the user.

// scope: owner
// row: hods the global state of the user for a single token

struct credit_assignation {
    name cat;
    asset stake;
    std::string to_string() const {
        return cat.to_string() + ": " + stake.to_string();
    }
};

TABLE mystake_table {
    asset total_stake;                             // Total amount of tokens in mystake for this account in all pools together
    asset total_mature;                            // Total amount of maured tokens ready to withdraw.
    std::vector<credit_assignation> credits;       // Distribution of credits among the categories defined in stakeconf
    time_point_sec credits_update;                 // Timestamp for last change in credits. Next change must respect stakeconf.credits_locktime;

    uint64_t primary_key()const { return total_stake.symbol.code().raw(); }
};

typedef eosio::multi_index<"mystake"_n, mystake_table> mystake;
