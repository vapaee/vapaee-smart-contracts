#!/usr/bin/env python3

from .constants import TelosProfile


def test_chgprofile(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'chgprofile',
        [alias, f'{alias}_'],
        f'{account}@active'
    )
    assert ec == 0

    profile = TelosProfile.get_profile(eosio_testnet, alias)
    assert profile is None

    profile = TelosProfile.get_profile(eosio_testnet, f'{alias}_')

    assert profile is not None
    assert account in profile['owners']


def test_chgprofile_profile_not_found(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'chgprofile',
        ['not a profile', 'not a profile'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found' in out


def test_chgprofile_identical_exists(eosio_testnet):
    account_a, alias_a = TelosProfile.new_profile(eosio_testnet)
    account_b, alias_b = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'chgprofile',
        [alias_a, alias_b],
        f'{account_a}@active'
    )
    assert ec == 1
    assert b'identical profile exists' in out