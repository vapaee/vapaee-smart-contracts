#!/usr/bin/env python3

import pytest

from .constants import TelosProfile, telosprofile


def test_addorg(telosprofile):
    """Create organization and check respective tables for correct update
    """
    account, alias = telosprofile.new_profile()
    org_name, symbols = telosprofile.add_organization(alias, assets=True)

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

    member = telosprofile.get_member(org_name, alias)
    assert member is not None
    assert TelosProfile.ORG_CREATOR in member['roles']


def test_addorg_profile_not_found(telosprofile):
    """Attempt to create an organization with a non existent creator profile,
    check for correct error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        ['not an alias', 'vapaee'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found' in out


def test_addorg_not_authorized(telosprofile):
    """Attempt to create an organization without the proper signature, check
    for correct error message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        [alias, 'vapaee'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized' in out


def test_addorg_organization_exists(telosprofile):
    """Attempt to create an organization with the same name as another, already
    existent organization, check for error message
    """
    account, alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(alias)
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        [alias, org_name],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'organization exists' in out

