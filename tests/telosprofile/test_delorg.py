#!/usr/bin/env python3

import pytest

from .constants import TelosProfile


def test_delorg(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, account, alias)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [alias, org_name],
        f'{account}@active'
    )
    assert ec == 0   

    org = TelosProfile.get_organization(eosio_testnet, org_name)
    assert org is None


def test_delorg_profile_not_found(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        ['not an alias', 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found' in out


def test_delorg_not_authorized(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [alias, 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert b'not authorized' in out


def test_delorg_organization_not_found(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [alias, 'vapaee'],
        f'{account}@active'
    )
    assert ec == 1
    assert b'organization not found' in out


def test_delorg_mustnt_have_members(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    other_account, other_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    TelosProfile.add_member(
        eosio_testnet,
        creat_account,
        creat_alias,
        org_name,
        other_alias
    )

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [creat_alias, org_name],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert b'organization mustn\'t have members'
