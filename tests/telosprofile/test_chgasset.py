#!/usr/bin/env python3

import random

from .constants import TelosProfile, telosprofile


def test_chgasset(telosprofile):
    """Create a new organization & add new member, use chgasset call to add 
    1000 units to each organization asset, then use chgasset again to subtract
    random expenses, check member asset tables to see the correct total for
    each asset.
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name, symbols = telosprofile.add_organization(creat_alias, assets=True)

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        user_alias
    )

    expenses = [
        random.randint(1,100)
        for _ in range(len(TelosProfile.org_asset_fields))
    ]
    for symbol, expense, asset_field, in zip(
        symbols,
        expenses,
        TelosProfile.org_asset_fields
    ):
        ec, out = telosprofile.testnet.push_action(
            TelosProfile.contract_name,
            'chgasset',
            [
                creat_alias,
                org_name,
                asset_field,
                'add',
                f'1000 {symbol}',
                user_alias
            ],
            f'{creat_account}@active'
        )
        assert ec == 0

        ec, out = telosprofile.testnet.push_action(
            TelosProfile.contract_name,
            'chgasset',
            [
                creat_alias,
                org_name,
                asset_field,
                'sub',
                f'{expense} {symbol}',
                user_alias
            ],
            f'{creat_account}@active'
        )
        assert ec == 0

    member = telosprofile.get_member(org_name, user_alias)
    assert member is not None

    for expense, asset_field in zip(
        expenses,
        TelosProfile.org_asset_fields
    ):
        balance = int(member[asset_field].split(' ')[0])
        assert balance == (1000 - expense)


def test_chgasset_profile_not_found_admin(telosprofile):
    """Attempt to change asset using a non extistent profile, check for correct
    error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            'not an alias',
            'not an org',
            'none',
            'none',
            '0 UNUSED',
            'not an alias'
        ],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_chgasset_profile_not_found_user(telosprofile):
    """Attempt to change asset of a non extistent profile, check for correct
    error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            creat_alias,
            'not an org',
            'none',
            'none',
            '0 UNUSED',
            'not an alias'
        ],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found (user)' in out


def test_chgasset_not_authorized_sig(telosprofile):
    """Attempt to change asset using the wrong signature, check for correct
    error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            creat_alias,
            'not an org',
            'none',
            'none',
            '0 UNUSED',
            user_alias
        ],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_chgasset_organization_not_found(telosprofile):
    """Attempt to change asset inside a non existent organization, check for
    correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            creat_alias,
            'not an org',
            'none',
            'none',
            '0 UNUSED',
            user_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'organization not found' in out


def test_chgasset_not_a_member_admin(telosprofile):
    """Attempt to change asset using an account that isn't a member, check for
    correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    bad_account, bad_alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            bad_alias,
            org_name,
            'none',
            'none',
            '0 UNUSED',
            user_alias
        ],
        f'{bad_account}@active'
    )
    assert ec == 1
    assert 'not a member (admin)' in out


def test_chgasset_not_authorized_org(telosprofile):
    """Attempt to change asset using a member account that isn't an admin,
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

    bad_account, bad_alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            user_alias,
            org_name,
            'none',
            'none',
            '0 UNUSED',
            bad_alias
        ],
        f'{user_account}@active'
    )
    assert ec == 1
    assert 'not authorized (org)' in out


def test_chgasset_not_a_member_user(telosprofile):
    """Attempt to change asset of an account that isn't a member, check for
    correct error message
    """
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            creat_alias,
            org_name,
            'none',
            'none',
            '0 UNUSED',
            user_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'not a member (user)' in out


def test_chgasset_creator_permission_required(telosprofile):
    """Attempt to change asset of creator account using an admin account, check for
    correct error message
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
        'chgasset',
        [
            admin_alias,
            org_name,
            'none',
            'none',
            '0 UNUSED',
            creat_alias
        ],
        f'{admin_account}@active'
    )
    assert ec == 1
    assert 'creator permission required' in out


def test_chgasset_invalid_operator(telosprofile):
    """Attempt to change asset using ``none`` operator (only valid ones are
    ``add`` & ``sub``), check for correct error message
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

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chgasset',
        [
            creat_alias,
            org_name,
            'none',
            'none',
            '0 UNUSED',
            user_alias
        ],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'invalid operator' in out

