#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile


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