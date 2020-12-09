#!/usr/bin/env python3

from .constants import TelosProfile


def test_addorg(eosio_testnet):
    account, alias = TelosProfile.new_profile(eosio_testnet)
    org_name = 'vapaee'

    TelosProfile.add_organization(eosio_testnet, account, alias, org_name)

    profile = TelosProfile.get_profile(eosio_testnet, alias)

    orgs = eosio_testnet.get_table(
        TelosProfile.contract_name,
        TelosProfile.contract_name,
        'orgs'
    )

    org = next((
        row for row in orgs['rows']
        if row['org_name'] == org_name),
        None
    )

    assert org is not None
    assert len(org['members']) == 1
    assert profile['id'] in org['members']


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
    org_name = 'bob & co'

    TelosProfile.add_organization(eosio_testnet, account, alias, org_name)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addorg',
        [alias, org_name],
        f'{account}@active'
    )
    assert ec == 1
    assert b'organization exists' in out