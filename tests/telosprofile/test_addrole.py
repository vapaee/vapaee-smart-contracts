#!/usr/bin/env python3

from .constants import TelosProfile


def test_addrole(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    admin_account, admin_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    TelosProfile.add_member(
        eosio_testnet,
        creat_account,
        creat_alias,
        org_name,
        admin_alias
    )

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [
            creat_alias,
            org_name,
            TelosProfile.ORG_ADMINISTRATOR,
            admin_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 0

    admin_profile = TelosProfile.get_profile(eosio_testnet, admin_alias)

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

    members = eosio_testnet.get_table(
        TelosProfile.contract_name,
        str(org['id']),
        'members'
    )

    member = next((
        row for row in members['rows']
        if row['profile_id'] == admin_profile['id']),
        None
    )

    assert member is not None
    assert TelosProfile.ORG_ADMINISTRATOR in member['roles']

    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    role_name = 'newfulluser'

    TelosProfile.add_member(
        eosio_testnet,
        admin_account,
        admin_alias,
        org_name,
        user_alias
    )

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [
            admin_alias,
            org_name,
            role_name,
            user_alias
        ],
        f'{admin_account}@active'
    )
    assert ec == 0

    user_profile = TelosProfile.get_profile(eosio_testnet, user_alias)

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

    members = eosio_testnet.get_table(
        TelosProfile.contract_name,
        str(org['id']),
        'members'
    )

    member = next((
        row for row in members['rows']
        if row['profile_id'] == user_profile['id']),
        None
    )

    assert member is not None
    assert role_name in member['roles']


def test_addrole_cant_give_that_role(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        ['not an alias', 'not an org', 'creator', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert b'can\'t give that role' in out


def test_addrole_profile_not_found_admin(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        ['not an alias', 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found (admin)' in out


def test_addrole_profile_not_found_user(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found (user)' in out


def test_addrole_not_authorized_sig(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        'eosio@active'
    )
    assert ec == 1
    assert b'not authorized (sig)' in out


def test_addrole_organization_not_found(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert b'organization not found' in out


def test_addrole_not_a_member_admin(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    bad_account, bad_alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [bad_alias, org_name, 'not.a.role', user_alias],
        f'{bad_account}@active'
    )
    assert ec == 1
    assert b'not a member (admin)' in out


def test_addrole_not_authorized_org(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    TelosProfile.add_member(
        eosio_testnet,
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    bad_account, bad_alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [user_alias, org_name, 'not.a.role', bad_alias],
        f'{user_account}@active'
    )
    assert ec == 1
    assert b'not authorized (org)' in out


def test_addrole_not_a_member_user(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, org_name, 'not.a.role', user_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert b'not a member (user)' in out


def test_addrole_user_has_the_role(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    TelosProfile.add_member(
        eosio_testnet,
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    role_name = 'newfulluser'
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [
            creat_alias,
            org_name,
            role_name,
            user_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 0

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [
            creat_alias,
            org_name,
            role_name,
            user_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert b'user has the role' in out