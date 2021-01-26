#!/usr/bin/env python3

from .constants import EventTracker, eventtracker
from ..telosprofile.constants import telosprofile


def test_addtarget(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()
    map_name = eventtracker.add_map(alias)
    season_name = eventtracker.add_season(alias)
    conf = '{\'test\': 1}'
    lat = '-123132.21323 LAT'
    lng = '124673.123131 LONG'

    target_name = eventtracker.add_target(
        alias, season_name, map_name,
        config=conf,
        lat=lat,
        lng=lng
    )

    map_info = eventtracker.get_map(map_name)
    season_info = eventtracker.get_season(season_name)
    target_info = eventtracker.get_target(season_name, target_name)

    assert target_info is not None
    assert target_info['season'] == season_info['id']
    assert target_info['config'] == conf
    assert target_info['map'] == map_info['id']
    assert target_info['lat'] == lat
    assert target_info['lng'] == lng


def test_addtarget_profile_not_found(eventtracker):
    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtarget',
        [
            'not an alias',
            'not a title',
            'not a title',
            '{}',
            'not a title',
            '0 LAT',
            '0 LONG'
        ],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_addtarget_not_authorized_sig(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtarget',
        [
            alias,
            'not a title',
            'not a title',
            '{}',
            'not a title',
            '0 LAT',
            '0 LONG'
        ],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized (sig)' in out


def test_addtarget_season_not_found(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtarget',
        [
            alias,
            'not a title',
            'not a title',
            '{}',
            'not a title',
            '0 LAT',
            '0 LONG'
        ],
        f'{account}@active'
    )
    assert ec == 1
    assert 'season not found' in out


def test_addtarget_not_authorized_wlist(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()
    other_account, other_alias = telosprofile.new_profile()

    season_name = eventtracker.add_season(other_alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtarget',
        [
            alias,
            season_name,
            'not a title',
            '{}',
            'not a title',
            '0 LAT',
            '0 LONG'
        ],
        f'{account}@active'
    )
    assert ec == 1
    assert 'not authorized (wlist)' in out


def test_addtarget_map_not_found(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()
    season_name = eventtracker.add_season(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtarget',
        [
            alias,
            season_name,
            'not a title',
            '{}',
            'not a title',
            '0 LAT',
            '0 LONG'
        ],
        f'{account}@active'
    )
    assert ec == 1
    assert 'map not found' in out


def test_addtarget_target_exists(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()

    map_name = eventtracker.add_map(alias)
    season_name = eventtracker.add_season(alias)
    target_name = eventtracker.add_target(alias, season_name, map_name)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addtarget',
        [
            alias,
            season_name,
            target_name,
            '{}',
            map_name,
            '0 LAT',
            '0 LONG'
        ],
        f'{account}@active'
    )
    assert ec == 1
    assert 'target exists' in out

