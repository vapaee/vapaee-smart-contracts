#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_setupprofile(telosprofile):
    account, alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(alias)

    contract = "vostok.dapps"

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'setupprofile',
        [
            alias,
            org_name,
            contract
        ],
        f'{account}@active'
    )
    assert ec == 0

    org_profile = telosprofile.get_profile(org_name)

    assert org_profile is not None
    assert org_profile['contract'] == contract
    assert contract in org_profile['owners']



def test_setupprofile_profile_not_found(telosprofile):
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'setupprofile',
        ['not an alias', 'not an org', 'notanaccount'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_setupprofile_not_authorized_sig(telosprofile):
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'setupprofile',
        [alias, 'not an org', 'notanaccount'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_setupprofile_organization_not_found(telosprofile):
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'setupprofile',
        [alias, 'not an org', 'notanaccount'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'organization not found' in out


def test_setupprofile_not_a_member_creator(telosprofile):
    account, alias = telosprofile.new_profile()
    bad_account, bad_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(alias)
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'setupprofile',
        [bad_alias, org_name, 'notanaccount'],
        f'{bad_account}@active'
    )
    assert ec == 1
    assert 'not a member (creator)' in out


def test_setupprofile_not_authorized_org(telosprofile):
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
        'setupprofile',
        [bad_alias, org_name, 'notanaccount'],
        f'{bad_account}@active'
    )
    assert ec == 1
    assert 'not authorized (org)' in out
