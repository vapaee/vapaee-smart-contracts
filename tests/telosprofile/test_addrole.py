#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_addrole(telosprofile):
    """Create organization & add user and give admin role, then make that
    admin add another user with role newfulluser, at heach step check the
    respective tables for correct updates
    """
    # creator & admin profiles
    creat_account, creat_alias = telosprofile.new_profile()
    admin_account, admin_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        admin_alias
    )

    # give role
    ec, out = telosprofile.testnet.push_action(
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

    member = telosprofile.get_member(org_name, admin_alias)
    assert member is not None
    assert TelosProfile.ORG_ADMINISTRATOR in member['roles']

    # user account setup
    user_account, user_alias = telosprofile.new_profile()
    role_name = 'newfulluser'

    telosprofile.add_member(
        admin_account,
        admin_alias,
        org_name,
        user_alias
    )

    # give role
    ec, out = telosprofile.testnet.push_action(
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

    member = telosprofile.get_member(org_name, user_alias)
    assert member is not None
    assert role_name in member['roles']


def test_addrole_cant_give_that_role(telosprofile):
    """Attempt to give the creator role, check error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        ['not an alias', 'not an org', 'creator', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert 'can\'t give that role' in out


def test_addrole_profile_not_found_admin(telosprofile):
    """Attempt to give a role with a non existent admin, check error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        ['not an alias', 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_addrole_profile_not_found_user(telosprofile):
    """Attempt to give a role to a non existant user, check error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found (user)' in out


def test_addrole_not_authorized_sig(telosprofile):
    """Attempt to give a role with the wrong signature, check error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_addrole_organization_not_found(telosprofile):
    """Attempt to give a role within a non existent organization, check error
    message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'organization not found' in out


def test_addrole_not_a_member_admin(telosprofile):
    """Attempt to give a role using an account that isn't a member, check error
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    bad_account, bad_alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [bad_alias, org_name, 'not.a.role', user_alias],
        f'{bad_account}@active'
    )
    assert ec == 1
    assert 'not a member (admin)' in out


def test_addrole_not_authorized_org(telosprofile):
    """Attempt to give a role using a non admin account, check error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    bad_account, bad_alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [user_alias, org_name, 'not.a.role', bad_alias],
        f'{user_account}@active'
    )
    assert ec == 1
    assert 'not authorized (org)' in out


def test_addrole_not_a_member_user(telosprofile):
    """Attempt to give a role to a non member, check error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, org_name, 'not.a.role', user_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'not a member (user)' in out


def test_addrole_creator_permission_required(telosprofile):
    """Attempt to give role to the creator using admin account, check error
    message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    admin_account, admin_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        admin_alias
    )

    ec, out = telosprofile.testnet.push_action(
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

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [
            admin_alias,
            org_name,
            'pranked',
            creat_alias
        ],
        f'{admin_account}@active'
    )
    assert ec == 1
    assert 'creator permission required' in out


def test_addrole_user_has_the_role(telosprofile):
    """Attempt to give the same role twice, check error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    role_name = 'newfulluser'
    ec, out = telosprofile.testnet.push_action(
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

    ec, out = telosprofile.testnet.push_action(
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
    assert 'user has the role' in out

