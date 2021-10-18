#!/usr/bin/env python3

from pytest_eosio import collect_stdout

from .constants import TelosProfile, telosprofile


def test_grantaccess(telosprofile):
    """Create two profiles, and attempt to call an action that uses the
    *telosprofile action permission system* using a profile in the name of the
    other, this should fail because we haven't setup the permissions.

    After setting up the permissions we check the respective tables for the
    correct values and attempt to do the call again, this time succeding
    """
    # create profiles
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    # attempt call without permissions & assert failure
    ec, out = telosprofile.testnet.push_action(
        'testcontract',
        'gaccesstest',
        [other_account, alias],
        f'{other_account}@active',
        retry=0
    )
    assert ec == 1 
    assert 'access denied' in out


    # give permissions
    ec, out = telosprofile.grant_access(
        alias, other_account, 'testcontract', 'gaccesstest'
    )
    assert ec == 0

    # check tables
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

    # try call again, should work
    ec, out = telosprofile.testnet.push_action(
        'testcontract',
        'gaccesstest',
        [other_account, alias],
        f'{other_account}@active'
    )
    assert ec == 0
    assert 'access granted' in collect_stdout(out)


def test_grantaccess_profile_not_found(telosprofile):
    """Attempt to grant access to a non profile, check for correct error
    message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'grantaccess',
        ['not an alias', 'notanaccount', 'notanaccount', 'notanaccount'],
        f'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found' in out


def test_grantaccess_not_authorized(telosprofile):
    """Attempt to grant access with wrong signature, check for correct error
    """
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'grantaccess',
        [alias, 'notanaccount', 'notanaccount', 'notanaccount'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized' in out
