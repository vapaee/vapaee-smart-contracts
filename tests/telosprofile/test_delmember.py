#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_delmember(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    # delete
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        [creat_alias, org_name, user_alias],
        f'{creat_account}@active'
    )
    assert ec == 0

    member = telosprofile.get_member(org_name, user_alias)
    assert member is None


def test_delmember_profile_not_found_admin(telosprofile):
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        ['not an alias', 'not an org', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_delmember_profile_not_found_user(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        [creat_alias, 'not an org', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found (user)' in out


def test_delmember_not_authorized_sig(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        [creat_alias, 'not an org', user_alias],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_delmember_organization_not_found(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        [creat_alias, 'not an org', user_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'organization not found' in out


def test_delmember_not_a_member_admin(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    bad_account, bad_alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        [bad_alias, org_name, user_alias],
        f'{bad_account}@active'
    )
    assert ec == 1
    assert 'not a member (admin)' in out


def test_delmember_not_authorized_org(telosprofile):
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
        'delmember',
        [user_alias, org_name, bad_alias],
        f'{user_account}@active'
    )
    assert ec == 1
    assert 'not authorized (org)' in out

def test_delrole_not_a_member_user(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        [creat_alias, org_name, user_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'not a member (user)' in out

def test_delrole_cant_delete_creator(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delmember',
        [creat_alias, org_name, creat_alias],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'can\'t delete creator' in out

