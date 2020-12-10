#!/usr/bin/env python3

import json
import string
import random

from typing import Optional

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

    ORG_CREATOR = 'creator'
    ORG_ADMINISTRATOR = 'admin'

    def new_profile(
        eosio_testnet,
        account_name: Optional[str] = None,
        alias_str: Optional[str] = None
    ):
        account = eosio_testnet.new_account(name=account_name)
        if not alias_str:
            alias = ''.join(
                random.choice(string.ascii_lowercase + string.digits)
                for _ in range(256)
            )
        else:
            alias = alias_str

        ec, out = eosio_testnet.push_action(
            TelosProfile.contract_name,
            'addprofile',
            [account, alias],
            f'{account}@active'
        )
        assert ec == 0
        return account, alias

    def get_profile(eosio_testnet, alias: str):
        searched_all = False
        profile = None
        while (not searched_all) and (not profile):
            profiles = eosio_testnet.get_table(
                TelosProfile.contract_name,
                TelosProfile.contract_name,
                'profiles',
                '-l', '1000'
            )

            profile = next((
                row for row in profiles['rows']
                if row['alias'] == alias),
                None
            )

            searched_all = not profiles['more']

        return profile

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
        searched_all = False
        link = None
        while (not searched_all) and (not link):
            links = eosio_testnet.get_table(
                TelosProfile.contract_name,
                str(profile['id']),
                'links'
            )

            link = next((
                row for row in links['rows']
                if row['token'] == proof),
                None
            )

            searched_all = not links['more']

        return link

    def get_link_with_id(eosio_testnet, alias: str, link_id: int):
        profile = TelosProfile.get_profile(eosio_testnet, alias)
        searched_all = False
        link = None
        while (not searched_all) and (not link):
            links = eosio_testnet.get_table(
                TelosProfile.contract_name,
                str(profile['id']),
                'links'
            )

            link = next((
                row for row in links['rows']
                if row['link_id'] == link_id),
                None
            )

            searched_all = not links['more']

        return link

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

    def add_organization(eosio_testnet, account: str, alias: str) -> str:
        org_name = ''.join(
            random.choice(string.ascii_lowercase + string.digits)
            for _ in range(32)
        )
        ec, out = eosio_testnet.push_action(
            TelosProfile.contract_name,
            'addorg',
            [alias, org_name],
            f'{account}@active'
        )
        assert ec == 0

        return org_name

    def add_member(eosio_testnet, account: str, admin_alias: str, org_name: str, user_alias: str):
        ec, out = eosio_testnet.push_action(
            TelosProfile.contract_name,
            'addmember',
            [admin_alias, org_name, user_alias],
            f'{account}@active'
        )
        assert ec == 0