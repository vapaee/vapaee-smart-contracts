#!/usr/bin/env python3

from .constants import TelosProfile


def test_addprofile(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)

    profile = TelosProfile.get_profile(eosio_testnet, alias)

    assert profile is not None
    assert profile['owner'] == account


def test_addprofile_exists(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addprofile',
        [account, alias],
        f'{account}@active'
    )
    assert ec == 1
    assert b'identical profile exists' in out