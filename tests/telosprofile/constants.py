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

    org_asset_fields = [f'index{i+1}' for i in range(5)]

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
        profiles = self.testnet.get_table(
            TelosProfile.contract_name,
            TelosProfile.contract_name,
            'profiles'
        )

        return next((
            row for row in profiles
            if row['alias'] == alias),
            None
        )

    def init_platforms(self):
        platforms = self.testnet.get_table(
            TelosProfile.contract_name,
            TelosProfile.contract_name,
            'platforms'
        )

        if len(platforms) > 0:
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
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'prooflink',
            [alias, str(link_id), url],
            f'{profile["owners"][0]}@active'
        )
        return ec, out

    def get_link_with_proof(self, alias: str, proof: str):
        profile = self.get_profile(alias)
        links = self.testnet.get_table(
            TelosProfile.contract_name,
            str(profile['id']),
            'links'
        )

        return next((
            row for row in links
            if row['token'] == proof),
            None
        )

    def get_link_with_id(self, alias: str, link_id: int):
        profile = self.get_profile(alias)
        links = self.testnet.get_table(
            TelosProfile.contract_name,
            str(profile['id']),
            'links'
        )

        return next((
            row for row in links
            if row['link_id'] == link_id),
            None
        )

    def witness_link(self, walias: str, lalias: str, link_id: int):
        wprofile = self.get_profile(walias)
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

    def grant_access(
        self,
        alias: str,
        target: str,
        contract: str,
        action: str
    ):
        profile = self.get_profile(alias)
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'grantaccess',
            [alias, target, contract, action],
            f'{profile["owners"][0]}@active'
        )
        return ec, out

    def revoke_access(
        self,
        alias: str,
        grant_id: int 
    ):
        profile = self.get_profile(alias)
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'revokeaccess',
            [alias, str(grant_id)],
            f'{profile["owners"][0]}@active'
        )
        return ec, out

    def add_organization(
        self,
        alias: str,
        assets: bool = False
    ) -> str:
        org_name = ''.join(
            random.choice(string.ascii_lowercase + string.digits)
            for _ in range(32)
        )

        profile = self.get_profile(alias)
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'addorg',
            [alias, org_name],
            f'{profile["owners"][0]}@active'
        )
        assert ec == 0
        
        if assets:
            symbols = [
                ''.join(
                    random.choice(string.ascii_uppercase)
                    for _ in range(3)
                ) for x in range(len(TelosProfile.org_asset_fields))
            ]
            for symbol, asset_field in zip(
                symbols,
                TelosProfile.org_asset_fields
            ):
                ec, out = self.init_org_asset(
                    alias,
                    org_name,
                    asset_field,
                    f'0 {symbol}'
                )

                assert ec == 0

            return org_name, symbols

        return org_name

    def init_org_asset(
        self,
        creat_alias: str,
        org_name: str,
        field: str,
        asset: str
    ):
        profile = self.get_profile(creat_alias)
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'initasset',
            [creat_alias, org_name, field, asset],
            f'{profile["owners"][0]}@active'
        )
        return ec, out

    def get_organization(self, org_name: str):
        orgs = self.testnet.get_table(
            TelosProfile.contract_name,
            TelosProfile.contract_name,
            'orgs'
        )

        return next((
            row for row in orgs
            if row['org_name'] == org_name),
            None
        )


    def add_member(self, account: str, admin_alias: str, org_name: str, user_alias: str):
        ec, out = self.testnet.push_action(
            TelosProfile.contract_name,
            'addmember',
            [admin_alias, org_name, user_alias],
            f'{account}@active'
        )
        assert ec == 0

    def get_member(self, org_name: str, user_alias: str):
        org = self.get_organization(org_name)
        user_profile = self.get_profile(user_alias)
        members = self.testnet.get_table(
            TelosProfile.contract_name,
            str(org['id']),
            'members'
        )

        return next((
            row for row in members
            if row['profile_id'] == user_profile['id']),
            None
        )




@pytest.fixture(scope="session")
def telosprofile(eosio_testnet):
    contract = TelosProfile(eosio_testnet)
    yield contract

