#!/usr/bin/env python3


def test_inverse(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        'testcontract',
        'inversetest',
        [],
        'eosio@active'
    )
    print(out)
    assert ec == 0


def test_multiply(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        'testcontract',
        'multiplytest',
        [],
        'eosio@active'
    )
    print(out)
    assert ec == 0
