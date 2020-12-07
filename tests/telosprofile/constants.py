#!/usr/bin/env python3

import string
import random

from pytest_eosiocdt import collect_stdout


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

    def add_link(eosio_testnet, alias: str,  platform: str, url: str):
        profile = TelosProfile.get_profile(eosio_testnet, alias)
        assert profile is not None
        ec, out = eosio_testnet.push_action(
            TelosProfile.contract_name,
            'addlink',
            [alias, platform, url],
            f'{profile["owners"][0]}@active'
        )
        assert ec == 0
        return collect_stdout(out)

    def get_link_with_proof(eosio_testnet, alias: str, proof: str):
        profile = TelosProfile.get_profile(eosio_testnet, alias)
        links = eosio_testnet.get_table(
            TelosProfile.contract_name,
            str(profile['id']),
            'links'
        )

        return next((
            row for row in links['rows']
            if row['token'] == proof),
            None
        )

    def get_link_with_id(eosio_testnet, alias: str, link_id: int):
        profile = TelosProfile.get_profile(eosio_testnet, alias)
        assert profile is not None
        links = eosio_testnet.get_table(
            TelosProfile.contract_name,
            str(profile['id']),
            'links'
        )

        return next((
            row for row in links['rows']
            if row['link_id'] == link_id),
            None
        )

    def witness_link(eosio_testnet, walias: str, lalias: str, link_id: int):
        wprofile = TelosProfile.get_profile(eosio_testnet, walias)
        assert wprofile is not None
        ec, out = eosio_testnet.push_action(
            TelosProfile.contract_name,
            'witness',
            [walias, lalias, str(link_id)],
            f'{wprofile["owners"][0]}@active'
        )
        assert ec == 0

    def update_profile(eosio_testnet, alias: str):
        ec, out = eosio_testnet.push_action(
            TelosProfile.contract_name,
            'updpoints',
            [alias],
            f'eosio@active'
        )
        assert ec == 0

    def add_organization(eosio_testnet, account: str, alias: str, org_name: str):
        ec, out = eosio_testnet.push_action(
            TelosProfile.contract_name,
            'addorg',
            [alias, org_name],
            f'{account}@active'
        )
        assert ec == 0