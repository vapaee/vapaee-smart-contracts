#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_addmember(telosprofile):
    """Create a new organization and add new member, check tables are correctly
    updated
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name, symbols = telosprofile.add_organization(
        creat_alias,
        assets=True
    )

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    member = telosprofile.get_member(org_name, user_alias)
    assert member is not None

    for symbol, asset_field in zip(
        symbols,
        TelosProfile.org_asset_fields
    ):
        assert member[asset_field] == f'0 {symbol}' 


def test_addmember_profile_not_found_admin(telosprofile):
    """Attempt to add a member using a non existent admin profile, check for
    correct error message
    """
    ec, out =  telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addmember',
        ['not an alias', 'not an org', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_addmember_profile_not_found_user(telosprofile):
    """Attempt to add a member that doesn't exist, check for correct error
    message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    
    ec, out =  telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addmember',
        [creat_alias, 'not an org', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (user)' in out


def test_addmember_not_authorized_sig(telosprofile):
    """Attempt to add a member using the wrong admin signature, check for
    correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out =  telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addmember',
        [creat_alias, 'not an org', user_alias],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_addmember_organization_not_found(telosprofile):
    """Attempt to add a member to a non existent organization, check for
    correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out =  telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addmember',
        [creat_alias, 'not an org', user_alias],
        f'{creat_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'organization not found' in out


def test_addmember_not_a_member_admin(telosprofile):
    """Attempt to add a member using an account that isn't even a member of the
    organization, check for correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    bad_account, bad_alias = telosprofile.new_profile()

    ec, out =  telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addmember',
        [bad_alias, org_name, user_alias],
        f'{bad_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not a member (admin)' in out


def test_addmember_not_authorized_org(telosprofile):
    """Attempt to add a member using an account that isn't an admin in the
    organization, check for correct error message
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

    ec, out =  telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addmember',
        [user_alias, org_name, bad_alias],
        f'{user_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (org)' in out


def test_addmember_already_a_member(telosprofile):
    """Attempt to add a member that already is a member of the organization,
    check for correct error message
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

    ec, out =  telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addmember',
        [creat_alias, org_name, user_alias],
        f'{creat_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'already a member' in out
