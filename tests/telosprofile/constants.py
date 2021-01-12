#!/usr/bin/env python3

import json
import string
import random

from typing import Optional

import pytest

from pytest_eosiocdt import collect_stdout


class TelosProfile:
    contract_name = 'telosprofile'
    platform_names = [
        'facebook',
        'twitter',
        'instagram',
        'youtube',
        'reddit',
        'steemit'
    ]

    org_asset_fields = [
        'points',
        'credits',
        'rewards',
        'trust',
        'rep'
    ]

    ORG_CREATOR = 'creator'
    ORG_ADMINISTRATOR = 'admin'

    def __init__(self, eosio_testnet):
        self.testnet = eosio_testnet

    def new_profile(
        self,
        account_name: Optional[str] = None,
        alias_str: Optional[str] = None
    ):
        account = self.testnet.new_account(name=account_name)
        if not alias_str:
            alias = ''.join(
                random.choice(string.ascii_lowercase + string.digits)
                for _ in range(256)
            )
        else:
            alias = alias_str

        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'addprofile',
            [account, alias],
            f'{account}@active'
        )
        assert ec == 0
        return account, alias

    def get_profile(self, alias: str):
        searched_all = False
        profile = None
        while (not searched_all) and (not profile):
            profiles = self.testnet.get_table(
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

    def init_platforms(self):

        platforms = self.testnet.get_table(
            TelosProfile.contract_name,
            TelosProfile.contract_name,
            'platforms'
        )

        if len(platforms['rows']) > 0:
            return

        for platform in TelosProfile.platform_names:
            ec, _ = self.testnet.push_action(
                TelosProfile.contract_name,
                'addplatform',
                [platform],
                f'{TelosProfile.contract_name}@active'
            )
            assert ec == 0

    def add_link(self, alias: str,  platform: str, url: str):
        profile = self.get_profile(alias)
        assert profile is not None
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'addlink',
            [alias, platform, url],
            f'{profile["owners"][0]}@active'
        )
        assert ec == 0
        return collect_stdout(out)

    def set_proof_link(self, alias: str, link_id: int, url: str):
        profile = self.get_profile(alias)
        assert profile is not None
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'prooflink',
            [alias, str(link_id), url],
            f'{profile["owners"][0]}@active'
        )
        return ec, out

    def get_link_with_proof(self, alias: str, proof: str):
        profile = self.get_profile(alias)
        searched_all = False
        link = None
        while (not searched_all) and (not link):
            links = self.testnet.get_table(
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

    def get_link_with_id(self, alias: str, link_id: int):
        profile = self.get_profile(alias)
        searched_all = False
        link = None
        while (not searched_all) and (not link):
            links = self.testnet.get_table(
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

    def witness_link(self, walias: str, lalias: str, link_id: int):
        wprofile = self.get_profile(walias)
        assert wprofile is not None
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'witness',
            [walias, lalias, str(link_id)],
            f'{wprofile["owners"][0]}@active'
        )
        assert ec == 0

    def update_profile(self, alias: str):
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'updpoints',
            [alias],
            f'eosio@active'
        )
        assert ec == 0

    def add_organization(self, account: str, alias: str) -> str:
        org_name = ''.join(
            random.choice(string.ascii_lowercase + string.digits)
            for _ in range(32)
        )
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'addorg',
            [alias, org_name],
            f'{account}@active'
        )
        assert ec == 0

        return org_name

    def init_org_asset(
        self,
        creat_alias: str,
        org_name: str,
        field: str,
        asset: str
    ):
        profile = self.get_profile(creat_alias)
        assert profile is not None
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'initasset',
            [creat_alias, org_name, field, asset],
            f'{profile["owners"][0]}@active'
        )
        return ec, out

    def get_organization(self, org_name: str):
        searched_all = False
        org = None
        while (not searched_all) and (not org):
            orgs = self.testnet.get_table(
                TelosProfile.contract_name,
                TelosProfile.contract_name,
                'orgs',
                '-l', '1000'
            )

            org = next((
                row for row in orgs['rows']
                if row['org_name'] == org_name),
                None
            )

            searched_all = not orgs['more']

        return org

    def add_member(self, account: str, admin_alias: str, org_name: str, user_alias: str):
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'addmember',
            [admin_alias, org_name, user_alias],
            f'{account}@active'
        )
        assert ec == 0


@pytest.fixture(scope="session")
def telosprofile(eosio_testnet):
    contract = TelosProfile(eosio_testnet)
    yield contract

