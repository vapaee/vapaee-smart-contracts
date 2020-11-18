#!/usr/bin/env python3

import string
import random


class TelosProfile:
	contract_name = "telosprofile"
	platform_names = [
		"facebook",
	    "twitter",
	    "instagram",
	    "youtube",
	    "reddit"
	]

	def new_profile(eosio_testnet):
	    account = eosio_testnet.new_account()
	    alias = ''.join(
	        random.choice(string.ascii_lowercase + string.digits)
	        for _ in range(256)
	    )

	    ec, out = eosio_testnet.push_action(
	        TelosProfile.contract_name,
	        'addprofile',
	        [account, alias],
	        f'{account}@active'
	    )
	    assert ec == 0
	    return account, alias
