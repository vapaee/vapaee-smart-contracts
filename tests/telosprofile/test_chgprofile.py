#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_chgprofile(telosprofile):
    """Create profile, then change alias, check for correct table updates
    """
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgprofile',
        [alias, f'{alias}_'],
        f'{account}@active'
    )
    assert ec == 0

    profile = telosprofile.get_profile(alias)
    assert profile is None

    profile = telosprofile.get_profile(f'{alias}_')

    assert profile is not None
    assert account in profile['owners']


def test_chgprofile_profile_not_found(telosprofile):
    """Attempt to change a non existent profile
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgprofile',
        ['not a profile', 'not a profile'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found' in out


def test_chglink_profile_not_authorized(telosprofile):
    """Attempt to change a profile with the wrong signature, check for correct
    error message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgprofile',
        [alias, 'not a profile'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized' in out


def test_chgprofile_identical_exists(telosprofile):
    """Attempt to change a profile alias to one in use, check for correct error
    message
    """
    account_a, alias_a = telosprofile.new_profile()
    account_b, alias_b = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgprofile',
        [alias_a, alias_b],
        f'{account_a}@active',
        retry=0
    )
    assert ec == 1
    assert 'identical profile exists' in out
