#!/usr/bin/env python3

import random

from .constants import TelosProfile, telosprofile


def test_chgasset(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name, symbols = telosprofile.add_organization(
        creat_account, creat_alias, assets=True
    )

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


    org = telosprofile.get_organization(org_name)

    members = telosprofile.testnet.get_table(
        TelosProfile.contract_name,
        str(org['id']),
        'members'
    )

    profile = telosprofile.get_profile(user_alias)

    member = next((
        row for row in members['rows']
        if row['profile_id'] == profile['id']),
        None
    )

    assert member is not None

    for expense, asset_field in zip(
        expenses,
        TelosProfile.org_asset_fields
    ):
        balance = int(member[asset_field].split(' ')[0])
        assert balance == (1000 - expense)


def test_chgasset_profile_not_found_admin(telosprofile):
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
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_account, creat_alias)

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
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_account, creat_alias)

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
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_account, creat_alias)

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
    creat_account, creat_alias = telosprofile.new_profile()
    admin_account, admin_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_account, creat_alias)

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
    creat_account, creat_alias = telosprofile.new_profile()
    user_account, user_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_account, creat_alias)

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
