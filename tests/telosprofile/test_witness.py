#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile


def test_witness(eosio_testnet):
    TelosProfile.init_platforms(eosio_testnet)

    # create root profile and register link
    account_link, alias_link = TelosProfile.new_profile(eosio_testnet)

    proof = TelosProfile.add_link(
        eosio_testnet,
        alias_link,  'facebook', 'https://localhost/facebook.html'
    )

    link = TelosProfile.get_link_with_proof(eosio_testnet, alias_link, proof)

    assert link is not None
    assert link['points'] == 0

    # create several accounts that witness the link
    link_id = link['link_id']

    bot_accounts = 5

    for i in range(bot_accounts):
        account_witness, alias_witness = TelosProfile.new_profile(eosio_testnet)

        TelosProfile.witness_link(eosio_testnet, alias_witness, alias_link, link_id)

        link = TelosProfile.get_link_with_id(eosio_testnet, alias_link, link_id)

        assert link is not None
        assert link['points'] == (i + 1)

    # update root profile points
    TelosProfile.update_profile(eosio_testnet, alias_link)

    # register new profile and link
    new_account, new_alias = TelosProfile.new_profile(eosio_testnet)
    proof = TelosProfile.add_link(
        eosio_testnet,
        new_alias,  'facebook', 'https://localhost/facebook.html'
    )

    link = TelosProfile.get_link_with_proof(eosio_testnet, new_alias, proof)

    assert link is not None

    # witness this new link with root account
    link_id = link['link_id']
    TelosProfile.witness_link(eosio_testnet, alias_link, new_alias, link_id)

    link = TelosProfile.get_link_with_id(eosio_testnet, new_alias, link_id)

    wprofile = TelosProfile.get_profile(eosio_testnet, alias_link)

    assert link is not None
    assert link['points'] == wprofile['points']


def test_witness_profile_not_found_witness(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        ['not a profile', 'not a profile', '0'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found (witness)' in out


def test_witness_not_authorized(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        [alias, 'not a profile', '0'],
        'eosio@active'
    )
    assert ec == 1
    assert b'not authorized' in out


def test_witness_profile_not_found_link(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        [alias, 'not a profile', '0'],
        f'{account}@active'
    )
    assert ec == 1
    assert b'profile not found (link)' in out


def test_witness_link_not_found(eosio_testnet):
    waccount, walias = TelosProfile.new_profile(eosio_testnet)
    laccount, lalias = TelosProfile.new_profile(eosio_testnet)
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        [walias, lalias, '6920105956'],
        f'{waccount}@active'
    )
    assert ec == 1
    assert b'link not found' in out