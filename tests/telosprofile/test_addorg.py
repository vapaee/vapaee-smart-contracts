#!/usr/bin/env python3

import pytest

from .constants import TelosProfile


def test_addorg(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, account, alias)

    org = TelosProfile.get_organization(eosio_testnet, org_name) 

    members = eosio_testnet.get_table(
        TelosProfile.contract_name,
        str(org['id']),
        'members'
    )

    profile = TelosProfile.get_profile(eosio_testnet, alias)

    member = next((
        row for row in members['rows']
        if row['profile_id'] == profile['id']),
        None
    )

    assert member is not None
    assert TelosProfile.ORG_CREATOR in member['roles']


def test_addorg_profile_not_found(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        ['not an alias', 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found' in out


def test_addorg_not_authorized(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        [alias, 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert b'not authorized' in out


def test_addorg_organization_exists(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, account, alias)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        [alias, org_name],
        f'{account}@active'
    )
    assert ec == 1
    assert b'organization exists' in out
