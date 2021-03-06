#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_revokeaccess(telosprofile):
    """Create two profiles, using one of them grant access to
    ``testcontract.gaccesstest``, check permission tables for correct updates,
    then revoke the access, check tables again, then attempt to call the action
    which should fail
    """
    # create accounts
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

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

    # revoke permissions
    ec, out = telosprofile.revoke_access(alias, grant['id'])
    assert ec == 0

    # attempt call
    ec, out = telosprofile.testnet.push_action(
        'testcontract',
        'gaccesstest',
        [other_account, alias],
        f'{other_account}@active'
    )
    assert ec == 1 
    assert 'access denied' in out


def test_revokeaccess_profile_not_found(telosprofile):
    """Attempt to revoke action permissions of non existent profiles, check
    correct error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'revokeaccess',
        ['notanalias', 0],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found' in out


def test_revokeaccess_not_authorized(telosprofile):
    """Attempt to revoke access using the wrong signature, check for correct
    error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'revokeaccess',
        [alias, 0],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized' in out
