#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile


def test_addlink(eosio_testnet):
    TelosProfile.init_platforms(eosio_testnet)
    account, alias = TelosProfile.new_profile(eosio_testnet)

    url = 'https://localhost/facebook.html'
    proof = TelosProfile.add_link(
        eosio_testnet,
        alias, 'facebook', url
    )
    assert len(proof) == 12

    link = TelosProfile.get_link_with_proof(eosio_testnet, alias, proof)

    assert link is not None
    assert link['url'] == url


def test_addlink_profile_not_found(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        ['not an alias', 'facebook', 'https://localhost/facebook.html'],
        f'eosio@active'
    )
    assert ec == 1
    assert b'profile not found' in out


def test_addlink_platform_not_found(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'parler', 'https://localhost/parler.html'],
        f'{account}@active'
    )
    assert ec == 1
    assert b'platform not found' in out


def test_addlink_already_exists(eosio_testnet):
    TelosProfile.init_platforms(eosio_testnet)
    account, alias = TelosProfile.new_profile(eosio_testnet)

    TelosProfile.add_link(
        eosio_testnet,
        alias,  'facebook', 'https://localhost/facebook.html'
    )

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 1
    assert b'link for this platform already exists' in out