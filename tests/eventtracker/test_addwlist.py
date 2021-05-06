#!/usr/bin/env python3

from .constants import EventTracker, eventtracker
from ..telosprofile.constants import telosprofile


def test_addwlist(telosprofile, eventtracker):
    """Create profile, map, season & target, create another profile and add it
    to the target event creators whitelist, check tables for updates
    """
    account, alias = telosprofile.new_profile()

    map_name = eventtracker.add_map(alias)
    season_name = eventtracker.add_season(alias)
    target_name = eventtracker.add_target(
        alias,
        season_name,
        map_name
    )

    other_account, other_alias = telosprofile.new_profile()
    other_profile = telosprofile.get_profile(other_alias)

    ec, out = eventtracker.add_to_target_whitelist(
        alias,
        season_name,
        target_name,
        other_alias
    )
    assert ec == 0

    target_info = eventtracker.get_target(season_name, target_name)

    assert target_info is not None
    assert other_profile['id'] in target_info['whitelist']


def test_addwlist_profile_not_found_admin(eventtracker):
    """Attempt to add profile to event creator whitelist using non existent
    profile, check error message
    """
    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addwlist',
        ['not an alias', 'not a title', 'not a title', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (admin)' in out


def test_addwlist_not_authorized_sig(telosprofile, eventtracker):
    """Attempt to add profile to event creator whitelist using the wrong
    signature, check error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addwlist',
        [alias, 'not a title', 'not a title', 'not an alias'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_addwlist_profile_not_found_new(telosprofile, eventtracker):
    """Attempt to add non existent profile to event creator whitelist, check
    error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addwlist',
        [alias, 'not a title', 'not a title', 'not an alias'],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (new)' in out


def test_addwlist_season_not_found(telosprofile, eventtracker):
    """Attempt to add profile to event creator whitelist using non existent
    season, check error message
    """
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addwlist',
        [alias, 'not a title', 'not a title', other_alias],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'season not found' in out


def test_addwlist_target_not_found(telosprofile, eventtracker):
    """Attempt to add profile to event creator whitelist using non existent
    target, check error message
    """
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    season_name = eventtracker.add_season(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addwlist',
        [alias, season_name, 'not a title', other_alias],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'target not found' in out


def test_addwlist_not_authorized_wlist(telosprofile, eventtracker):
    """Attempt to add profile to event creator whitelist using non admin
    profile, check error message
    """
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    map_name = eventtracker.add_map(alias)
    season_name = eventtracker.add_season(alias)
    target_name = eventtracker.add_target(
        alias,
        season_name,
        map_name
    )

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addwlist',
        [other_alias, season_name, target_name, other_alias],
        f'{other_account}@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized (wlist)' in out

