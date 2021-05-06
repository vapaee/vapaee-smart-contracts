#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_delrole(telosprofile):
    """Create organization, add member and give ``newfulluser`` role, then
    delete that role, check tables for correct update
    """
    creat_account, creat_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    user_account, user_alias = telosprofile.new_profile()
    role_name = 'newfulluser'

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

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

    member = telosprofile.get_member(org_name, user_alias)
    assert member is not None
    assert role_name not in member['roles']


def test_delrole_profile_not_found_admin(telosprofile):
    """Attempt to delete a role using a non existent admin profile, check for
    correct error code
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        ['not an alias', 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_delrole_profile_not_found_user(telosprofile):
    """Attempt to delete a role from a user that doesn't exist, check for
    correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [creat_alias, 'not an org', 'not.a.role', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (user)' in out


def test_delrole_not_authorized_sig(telosprofile):
    """Attempt to delete a role using the wrong signature, check for correct
    error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_delrole_organization_not_found(telosprofile):
    """Attempt to delete a role from a member of an organization that doesn't
    exist, check for correct error
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [creat_alias, 'not an org', 'not.a.role', user_alias],
        f'{creat_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'organization not found' in out


def test_delrole_not_a_member_admin(telosprofile):
    """Attempt to delete a role using a non admin profile, check for correct
    error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    bad_account, bad_alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [bad_alias, org_name, 'not.a.role', user_alias],
        f'{bad_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not a member (admin)' in out


def test_delrole_not_authorized_org(telosprofile):
    """Attempt to delete a role from a non member profile, check for correct
    error message
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
        'delrole',
        [user_alias, org_name, 'not.a.role', bad_alias],
        f'{user_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (org)' in out


def test_delrole_not_a_member_user(telosprofile):
    """Attempt to delete role of non member, check for correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addrole',
        [creat_alias, org_name, 'not.a.role', user_alias],
        f'{creat_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not a member (user)' in out

def test_delrole_creator_permission_required(telosprofile):
    """Attempt to delete role from creator using administator profile, check
    for correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    admin_account, admin_alias = telosprofile.new_profile()

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
        'delrole',
        [
            admin_alias,
            org_name,
            TelosProfile.ORG_CREATOR,
            creat_alias
        ],
        f'{admin_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'creator permission required' in out

def test_delrole_user_doesnt_have_the_role(telosprofile):
    """Attempt to delete a role that the member doesn't have, check for correct
    error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    user_account, user_alias = telosprofile.new_profile()
    role_name = 'newfulluser'

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delrole',
        [
            creat_alias,
            org_name,
            role_name,
            user_alias
        ],
        f'{creat_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'user doesn\'t have the role' in out

