#!/usr/bin/env python3

import string
import random

from hashlib import sha256


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

    def get_profile(eosio_testnet, alias: str):
        profiles = eosio_testnet.get_table(
            TelosProfile.contract_name,
            TelosProfile.contract_name,
            'profiles'
        )

        return next((
            row for row in profiles['rows']
            if row['alias'] == alias),
            None
        )

    def init_platforms(eosio_testnet):

        platforms = eosio_testnet.get_table(
            TelosProfile.contract_name,
            TelosProfile.contract_name,
            'platforms'
        )

        if len(platforms['rows']) > 0:
            return

        for platform in TelosProfile.platform_names:
            ec, _ = eosio_testnet.push_action(
                TelosProfile.contract_name,
                'addplatform',
                [platform],
                f'{TelosProfile.contract_name}@active'
            )
            assert ec == 0