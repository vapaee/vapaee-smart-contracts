#!/usr/bin/env python3

import json

from .constants import EventTracker, eventtracker
from ..telosprofile.constants import telosprofile


def test_addevent(telosprofile, eventtracker):
    """Create new profile, map, season & target, as well as event information,
    add that event & check respective tables for correct values
    """
    # generate profile, map, season & target information
    account, alias = telosprofile.new_profile()

    lat = '-791123.1123 LAT'
    lng = '981234.7545 LONG'
    geom = json.dumps({'type': 'square'})
    conf = json.dumps({'_id': 92})

    map_name = eventtracker.add_map(alias)
    season_name = eventtracker.add_season(alias)
    target_name = eventtracker.add_target(
        alias,
        season_name,
        map_name
    )

    # add event 
    ec, _ = eventtracker.add_event(
        alias,
        season_name,
        target_name,
        map_name,
        lat=lat,
        lng=lng,
        geometry=geom,
        config=conf
    )

    assert ec == 0

    # check tables
    profile = telosprofile.get_profile(alias)
    map_info = eventtracker.get_map(map_name)
    target_info = eventtracker.get_target(season_name, target_name)
    events = eventtracker.get_events_with_config(
        season_name,
        target_name,
        _id=92
    )

    assert len(events) == 1

    event_info = events[0]

    assert event_info is not None
    assert event_info['target'] == target_info['id']
    assert event_info['profile'] == profile['id']
    assert event_info['map'] == map_info['id']
    assert event_info['lat'] == lat
    assert event_info['lng'] == lng
    assert event_info['geometry'] == geom
    assert event_info['config'] == conf


def test_addevent_profile_not_found(eventtracker):
    """Attempt to add an event using a non existent profile, check error
    message
    """
    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addevent',
        [
            'not an alias',
            'not a season',
            'not a target',
            'not a map',
            '0 LAT',
            '0 LONG',
            '{}',
            '{}'
        ],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_addevent_not_authorized_sig(telosprofile, eventtracker):
    """Attempt to add event using the wrong signature, check error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addevent',
        [
            alias,
            'not a season',
            'not a target',
            'not a map',
            '0 LAT',
            '0 LONG',
            '{}',
            '{}'
        ],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_addevent_season_not_found(telosprofile, eventtracker):
    """Attempt to add an event to a non existent season, check error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addevent',
        [
            alias,
            'not a season',
            'not a target',
            'not a map',
            '0 LAT',
            '0 LONG',
            '{}',
            '{}'
        ],
        f'{account}@active'
    )
    assert ec == 1
    assert 'season not found' in out


def test_addevent_map_not_found(telosprofile, eventtracker):
    """Attempt to add an event to a non existent map, check error message
    """
    account, alias = telosprofile.new_profile()
    season_name = eventtracker.add_season(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addevent',
        [
            alias,
            season_name,
            'not a target',
            'not a map',
            '0 LAT',
            '0 LONG',
            '{}',
            '{}'
        ],
        f'{account}@active'
    )
    assert ec == 1
    assert 'map not found' in out


def test_addevent_target_not_found(telosprofile, eventtracker):
    """Attempt to add an event to a non existent target, check error message
    """
    account, alias = telosprofile.new_profile()
    map_name = eventtracker.add_map(alias)
    season_name = eventtracker.add_season(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addevent',
        [
            alias,
            season_name,
            'not a target',
            map_name,
            '0 LAT',
            '0 LONG',
            '{}',
            '{}'
        ],
        f'{account}@active'
    )
    assert ec == 1
    assert 'target not found' in out


def test_addevent_not_authorized_wlist(telosprofile, eventtracker):
    """Attempt to add event using a profile that is not on the target whitelist
    check error message
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
        'addevent',
        [
            other_alias,
            season_name,
            target_name,
            map_name,
            '0 LAT',
            '0 LONG',
            '{}',
            '{}'
        ],
        f'{other_account}@active'
    )
    assert ec == 1
    assert 'not authorized (wlist)' in out

