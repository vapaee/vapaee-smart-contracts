#!/usr/bin/env python3

from .constants import TelosProfile


def test_addrole(eosio_testnet):
    ca_account, ca_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = 'roletest_org$2486571386'

    TelosProfile.add_organization(eosio_testnet, ca_account, ca_alias, org_name)

    sec_account, sec_alias = TelosProfile.new_profile(eosio_testnet)

    role_name = 'secondary'
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [
            ca_alias,
            org_name,
            role_name,
            sec_alias
        ],
        f'{ca_account}@active'
    )
    assert ec == 0

    # assert added to member list
    ca_profile = TelosProfile.get_profile(eosio_testnet, ca_alias)
    sec_profile = TelosProfile.get_profile(eosio_testnet, sec_alias)

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
    assert len(org['members']) == 2
    assert ca_profile['id'] in org['members']
    assert sec_profile['id'] in org['members']

    # assert role
    roles = eosio_testnet.get_table(
        TelosProfile.contract_name,
        str(org['id']),
        'roles'
    )

    print(roles['rows'])

    role = next((
        row for row in roles['rows']
        if row['profile_id'] == sec_profile['id']),
        None
    )
    assert role is not None
    assert role['role_name'] == role_name