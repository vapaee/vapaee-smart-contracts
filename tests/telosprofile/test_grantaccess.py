#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile, telosprofile


def test_grantaccess(telosprofile):
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        'testcontract',
        'gaccesstest',
        [other_account, alias],
        f'{other_account}@active'
    )
    assert ec == 1 
    assert 'access denied' in out


    ec, out = telosprofile.grant_access(
        alias, other_account, 'testcontract', 'gaccesstest'
    )
    assert ec == 0

    profile = telosprofile.get_profile(alias)

    grants = telosprofile.testnet.get_table(
        TelosProfile.contract_name,
        str(profile['id']),
        'access'
    )

    grant = next((
        row for row in grants
        if (row['target'] == other_account) and
        (row['contract'] == 'testcontract') and
        (row['action'] == 'gaccesstest')),
        None
    )

    assert grant is not None

    ec, out = telosprofile.testnet.push_action(
        'testcontract',
        'gaccesstest',
        [other_account, alias],
        f'{other_account}@active'
    )
    assert ec == 0
    assert 'access granted' in collect_stdout(out)


def test_grantaccess_profile_not_found(telosprofile):
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'grantaccess',
        ['not an alias', 'notanaccount', 'notanaccount', 'notanaccount'],
        f'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_grantaccess_not_authorized(telosprofile):
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'grantaccess',
        [alias, 'notanaccount', 'notanaccount', 'notanaccount'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out
