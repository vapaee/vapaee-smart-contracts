#!/usr/bin/env python3

import pytest

from .constants import TelosProfile, telosprofile


def test_delorg(telosprofile):
    account, alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(alias)

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [alias, org_name],
        f'{account}@active'
    )
    assert ec == 0   

    org = TelosProfile.get_organization(telosprofile, org_name)
    assert org is None


def test_delorg_profile_not_found(telosprofile):
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        ['not an alias', 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_delorg_not_authorized(telosprofile):
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [alias, 'vapaee'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_delorg_organization_not_found(telosprofile):
    account, alias = telosprofile.new_profile()
    
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [alias, 'vapaee'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'organization not found' in out


def test_delorg_mustnt_have_members(telosprofile):
    creat_account, creat_alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()
    org_name = telosprofile.add_organization(creat_alias)

    telosprofile.add_member(
        creat_account,
        creat_alias,
        org_name,
        other_alias
    )

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'delorg',
        [creat_alias, org_name],
        f'{creat_account}@active'
    )
    assert ec == 1
    assert 'organization mustn\'t have members'
