#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile


def test_chglink(eosio_testnet):
    TelosProfile.init_platforms(eosio_testnet)
    account, alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 0
    proof_token = collect_stdout(out)

    profile = TelosProfile.get_profile(eosio_testnet, alias)

    links = eosio_testnet.get_table(
        TelosProfile.contract_name,
        str(profile['id']),
        'links'
    )

    link = next((
        row for row in links['rows']
        if row['token'] == proof_token),
        None
    )

    assert link is not None

    link_id = link['link_id']

    new_url = 'https://localhost/facebook2.html'

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        [alias, str(link_id), new_url],
        f'{account}@active'
    )
    assert ec == 0

    links = eosio_testnet.get_table(
        TelosProfile.contract_name,
        str(profile['id']),
        'links'
    )

    link = next((
        row for row in links['rows']
        if row['link_id'] == link_id),
        None
    )

    assert link is not None
    assert link['url'] == new_url


def test_chglink_profile_not_found(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        ['not a profile', '0', 'localhost'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_chglink_profile_not_authorized(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        [alias, '0', 'localhost'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_chglink_link_not_found(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        [alias, '0', 'localhost'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'link not found' in out