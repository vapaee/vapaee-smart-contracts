#!/usr/bin/env python3

from .constants import EventTracker, eventtracker
from ..telosprofile.constants import telosprofile


def test_addtcwlist(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    other_profile = telosprofile.get_profile(other_alias)

    season_name = eventtracker.add_season(alias)

    ec, _ = eventtracker.add_target_creator(alias, season_name, other_alias)

    assert ec == 0

    season_info = eventtracker.get_season(season_name)

    assert other_profile['id'] in season_info['tc_whitelist']


def test_addtcwlist_profile_not_found_admin(eventtracker):
    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        ['not an alias', 'not a title', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_addtcwlist_not_authorized_sig(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [alias, 'not a title', 'not an alias'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_addtcwlist_profile_not_found_new(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [alias, 'not a title', 'not an alias'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'profile not found (new)' in out


def test_addtcwlist_season_not_found(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [alias, 'not a title', other_alias],
        f'{account}@active'
    )
    assert ec == 1
    assert 'season not found' in out


def test_addtcwlist_not_authorized_wlist(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    season_name = eventtracker.add_season(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [other_alias, season_name, other_alias],
        f'{other_account}@active'
    )
    assert ec == 1
    assert 'not authorized (wlist)' in out

