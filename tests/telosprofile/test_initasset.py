#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_initasset(telosprofile):
    account, alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(account, alias)

    symbols = [
        'SYS', 'ROM', 'CRM', 'ATO', 'CCP'
    ]

    for symbol, asset_field in zip(
        symbols,
        TelosProfile.org_asset_fields
    ):
        ec, out = telosprofile.init_org_asset(
            alias,
            org_name,
            asset_field,
            f'1000 {symbol}'
        )

        assert ec == 0

    org = telosprofile.get_organization(org_name)

    for symbol, asset_field in zip(
        symbols,
        TelosProfile.org_asset_fields
    ):
        assert org[asset_field] == f'0 {symbol}'


def test_initasset_profile_not_found(telosprofile):
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        ['not an alias', 'vapaee', 'none', '0 CAT'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_initasset_not_authorized(telosprofile):
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [alias, 'vapaee', 'none', '0 CAT'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_initasset_organization_not_found(telosprofile):
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [alias, 'vapaee', 'none', '0 CAT'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'organization not found' in out


def test_initasset_invalid_field(telosprofile):
    account, alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(account, alias)
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [alias, org_name, 'none', '0 CAT'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'field must be one of (points credits rewards trust rep)' in out

