#!/usr/bin/env python3

import pytest

from .constants import TelosProfile, telosprofile


def test_addorg(telosprofile):
    account, alias = telosprofile.new_profile()
    org_name, symbols = telosprofile.add_organization(
        account, alias, assets=True
    )

    org = telosprofile.get_organization(org_name)
    org_profile = telosprofile.get_profile(org_name)

    assert org_profile is not None

    assert org['org_name'] == org_name
    assert org['profile'] == org_profile['id']
    
    for symbol, asset_field in zip(
        symbols,
        TelosProfile.org_asset_fields
    ):
        assert org[asset_field] == f'0 {symbol}'

    members = telosprofile.testnet.get_table(
        TelosProfile.contract_name,
        str(org['id']),
        'members'
    )

    profile = telosprofile.get_profile(alias)

    member = next((
        row for row in members['rows']
        if row['profile_id'] == profile['id']),
        None
    )

    assert member is not None
    assert TelosProfile.ORG_CREATOR in member['roles']


def test_addorg_profile_not_found(telosprofile):
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        ['not an alias', 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_addorg_not_authorized(telosprofile):
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        [alias, 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_addorg_organization_exists(telosprofile):
    account, alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(account, alias)
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        [alias, org_name],
        f'{account}@active'
    )
    assert ec == 1
    assert 'organization exists' in out
