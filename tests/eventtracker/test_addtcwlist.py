#!/usr/bin/env python3

from .constants import EventTracker, eventtracker
from ..telosprofile.constants import telosprofile


def test_addtcwlist(telosprofile, eventtracker):
    """Create profile & add season, then add other profile to target creator
    whitelist, check respective tables
    """
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    other_profile = telosprofile.get_profile(other_alias)

    season_name = eventtracker.add_season(alias)

    ec, _ = eventtracker.add_target_creator(alias, season_name, other_alias)

    assert ec == 0

    season_info = eventtracker.get_season(season_name)

    assert other_profile['id'] in season_info['tc_whitelist']


def test_addtcwlist_profile_not_found_admin(eventtracker):
    """Attempt to add profile to target creator whitelist using non exitent
    profile, check error message
    """
    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        ['not an alias', 'not a title', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_addtcwlist_not_authorized_sig(telosprofile, eventtracker):
    """Attempt to add profile to target creator whitelist using wrong signature
    check error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [alias, 'not a title', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_addtcwlist_profile_not_found_new(telosprofile, eventtracker):
    """Attempt to add non existent profile to target creator whitelist, check
    error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [alias, 'not a title', 'not an alias'],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (new)' in out


def test_addtcwlist_season_not_found(telosprofile, eventtracker):
    """Attempt to add profile to target creator whitelist of non existent
    season, check error message
    """
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [alias, 'not a title', other_alias],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'season not found' in out


def test_addtcwlist_not_authorized_wlist(telosprofile, eventtracker):
    """Attempt to add profile to target creator whitelist using non admin
    account, check error message
    """
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    season_name = eventtracker.add_season(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtcwlist',
        [other_alias, season_name, other_alias],
        f'{other_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (wlist)' in out

