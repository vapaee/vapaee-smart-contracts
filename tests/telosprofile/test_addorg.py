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


