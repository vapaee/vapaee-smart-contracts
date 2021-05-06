#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_addprofile(telosprofile):
    """Add new profile and check respective table
    """
    account, alias = telosprofile.new_profile()

    profile = telosprofile.get_profile(alias)

    assert profile is not None
    assert account in profile['owners']


def test_addprofile_exists(telosprofile):
    """Attempt to create a new profile with the same name as another profile,
    check for error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addprofile',
        [account, alias],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'identical profile exists' in out
