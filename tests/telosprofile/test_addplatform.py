#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_addplatform(telosprofile):
    telosprofile.init_platforms()

    platforms = telosprofile.testnet.get_table(
        TelosProfile.contract_name,
        TelosProfile.contract_name,
        'platforms'
    )

    registered_platforms = [row['pname'] for row in platforms]

    for platform in TelosProfile.platform_names:
        assert platform in registered_platforms


def test_addplatform_identical_exists(telosprofile):
    telosprofile.init_platforms()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addplatform',
        [TelosProfile.platform_names[0]],
        f'{TelosProfile.contract_name}@active'
    )
    assert ec == 1
    assert 'identical platform exists' in out
