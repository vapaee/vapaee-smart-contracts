#!/usr/bin/env python3

from .constants import TelosProfile


def test_addplatform(eosio_testnet):
    TelosProfile.init_platforms(eosio_testnet)

    platforms = eosio_testnet.get_table(
        TelosProfile.contract_name,
        TelosProfile.contract_name,
        'platforms'
    )

    registered_platforms = [row['pname'] for row in platforms['rows']]

    for platform in TelosProfile.platform_names:
        assert platform in registered_platforms


def test_addplatform_identical_exists(eosio_testnet):
    TelosProfile.init_platforms(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addplatform',
        [TelosProfile.platform_names[0]],
        f'{TelosProfile.contract_name}@active'
    )
    assert ec == 1
    assert b'identical platform exists' in out