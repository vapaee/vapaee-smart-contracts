#!/usr/bin/env python3

from .constants import TelosProfile


def test_delrole(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    role_name = 'newfulluser'

    TelosProfile.add_member(
        eosio_testnet,
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

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
        'delrole',
        [
            creat_alias,
            org_name,
            role_name,
            user_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 0

    user_profile = TelosProfile.get_profile(eosio_testnet, user_alias)

    org = TelosProfile.get_organization(eosio_testnet, org_name)

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
    assert role_name not in member['roles']


def test_delrole_profile_not_found_admin(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        ['not an alias', 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found (admin)' in out


def test_delrole_profile_not_found_user(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [creat_alias, 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert b'profile not found (user)' in out


def test_delrole_not_authorized_sig(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        'eosio@active'
    )
    assert ec == 1
    assert b'not authorized (sig)' in out


def test_delrole_organization_not_found(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    
    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert b'organization not found' in out


def test_delrole_not_a_member_admin(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    bad_account, bad_alias = TelosProfile.new_profile(eosio_testnet)

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [bad_alias, org_name, 'not.a.role', user_alias],
        f'{bad_account}@active'
    )
    assert ec == 1
    assert b'not a member (admin)' in out


def test_delrole_not_authorized_org(eosio_testnet):
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
        'delrole',
        [user_alias, org_name, 'not.a.role', bad_alias],
        f'{user_account}@active'
    )
    assert ec == 1
    assert b'not authorized (org)' in out


def test_delrole_not_a_member_user(eosio_testnet):
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

def test_delrole_creator_permission_required(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    admin_account, admin_alias = TelosProfile.new_profile(eosio_testnet)

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

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [
            admin_alias,
            org_name,
            TelosProfile.ORG_CREATOR,
            creat_alias
        ],
        f'{admin_account}@active'
    )
    assert ec == 1
    assert b'creator permission required' in out

def test_delrole_user_doesnt_have_the_role(eosio_testnet):
    creat_account, creat_alias = TelosProfile.new_profile(eosio_testnet)
    org_name = TelosProfile.add_organization(eosio_testnet, creat_account, creat_alias)

    user_account, user_alias = TelosProfile.new_profile(eosio_testnet)
    role_name = 'newfulluser'

    TelosProfile.add_member(
        eosio_testnet,
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    ec, out = eosio_testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [
            creat_alias,
            org_name,
            role_name,
            user_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert b'user doesn\'t have the role' in out

