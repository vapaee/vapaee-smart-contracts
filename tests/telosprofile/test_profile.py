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