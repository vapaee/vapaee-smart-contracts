#!/usr/bin/env python3
import string
import random

from .constants import TelosProfile


def new_profile(eosio_testnet):
    account = eosio_testnet.new_account()
    alias = ''.join(
        random.choice(string.ascii_lowercase + string.digits)
        for _ in range(256)
    )

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addprofile',
        [account, alias],
        f'{account}@active'
    )
    assert ec == 0
    return account, alias


def test_addprofile(eosio_testnet):
    account, alias = new_profile(eosio_testnet)

    profiles = eosio_testnet.get_table(
        TelosProfile.contract_name,
        TelosProfile.contract_name,
        'profiles'
    )

    row = next((
        row for row in profiles['rows']
        if row['owner'] == account),
        None
    )

    assert row is not None
    assert row['alias'] == alias


def test_addprofile_exists(eosio_testnet):
    account, alias = new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addprofile',
        [account, alias],
        f'{account}@active'
    )
    assert ec == 1
    assert b'identical profile exists' in out


def test_purgeprofile(eosio_testnet):
    account, alias = new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'purgeprofile',
        [alias],
        f'{account}@active'
    )
    assert ec == 0

    profiles = eosio_testnet.get_table(
        TelosProfile.contract_name,
        TelosProfile.contract_name,
        'profiles'
    )

    row = next((
        row for row in profiles['rows']
        if row['owner'] == account),
        None
    )

    assert row is None

def test_purgeprofile_not_found(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'purgeprofile',
        ['null'],
        f'eosio@active'
    )
    assert ec == 1
    assert b'profile not found' in out