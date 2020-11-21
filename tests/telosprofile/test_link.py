#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

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


def test_addlink(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 0
    assert len(collect_stdout(out)) == 12