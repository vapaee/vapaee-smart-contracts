// -- access --

// scope: profile.id
// row: represent a delegation of trust from profile to granted to execute a specific action on a specific smart contract.

TABLE grant_t {
    uint64_t       id; // auto-increment
    name       target; // account name to grant permission
    name     contract; // contract were the action is   
    name       action; // name of the action

    uint64_t primary_key() const {
        return id;
    }

    // encodes all parameters into a single signature
    // we are only using 192 bits of the total 256
    checksum256 encode_into_256() const {
        const array<uint128_t, 2> sig = {
            (uint128_t)target.value,
            ((uint128_t)contract.value << 64) | action.value
        };
        return fixed_bytes<32>(sig);
    }
};

typedef eosio::multi_index<"access"_n, grant_t,
    indexed_by<"accesscheck"_n, const_mem_fun<grant_t, checksum256, &grant_t::encode_into_256>>
> access;
