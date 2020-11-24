#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile


def test_addlink(eosio_testnet):
    TelosProfile.init_platforms(eosio_testnet)
    account, alias = TelosProfile.new_profile(eosio_testnet)

    url = 'https://localhost/facebook.html'

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', url],
        f'{account}@active'
    )
    assert ec == 0
    proof_token = collect_stdout(out)
    assert len(proof_token) == 12

    profile = TelosProfile.get_profile(eosio_testnet, alias)

    links = eosio_testnet.get_table(
        TelosProfile.contract_name,
        str(profile['id']),
        'links'
    )

    row = next((
        row for row in links['rows']
        if row['token'] == proof_token),
        None
    )

    assert row is not None
    assert row['url'] == url


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
    account, alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 0

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 1
    assert b'link for this platform already exists' in out