#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_initasset(telosprofile):
    """Create new organization and check assets for correct initial values
    """
    account, alias = telosprofile.new_profile()
    org_name, symbols = telosprofile.add_organization(alias, assets=True)

    org = telosprofile.get_organization(org_name)

    for symbol, asset_field in zip(
        symbols,
        TelosProfile.org_asset_fields
    ):
        assert org[asset_field] == f'0 {symbol}'


def test_initasset_profile_not_found(telosprofile):
    """Attempt to init an asset in a profile that doesn't exist, check error
    message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        ['not an alias', 'vapaee', 'none', '0 CAT'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found' in out


def test_initasset_not_authorized_sig(telosprofile):
    """Attempt to init an asset using the wrong signature, check error message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [alias, 'vapaee', 'none', '0 CAT'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_initasset_organization_not_found(telosprofile):
    """Attempt to init an asset of a non existent organization, check error
    message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [alias, 'vapaee', 'none', '0 CAT'],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'organization not found' in out


def test_initasset_not_a_member_creator(telosprofile):
    """Attempt to init an asset using a non member account, check error message
    """
    account, alias = telosprofile.new_profile()
    bad_account, bad_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(alias)
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [bad_alias, org_name, 'none', '0 CAT'],
        f'{bad_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not a member (creator)' in out


def test_initasset_not_authorized_org(telosprofile):
    """Attempt to init an asset using a member with no permission to do so,
    check error message
    """
    account, alias = telosprofile.new_profile()
    bad_account, bad_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(alias)

    telosprofile.add_member(
        account,
        alias,
        org_name,
        bad_alias
    )

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [bad_alias, org_name, 'none', '0 CAT'],
        f'{bad_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (org)' in out


def test_initasset_invalid_field(telosprofile):
    """Attempt to init an invalid asset, check error message
    """
    account, alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(alias)
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'initasset',
        [alias, org_name, 'none', '0 CAT'],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'field must be one of (index[1-5])' in out

