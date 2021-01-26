#!/usr/bin/env python3


from .constants import EventTracker, eventtracker
from ..telosprofile.constants import telosprofile


def test_addmap(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()
    
    profile = telosprofile.get_profile(alias)

    conf = '{\'test\': true}'
    map_name = eventtracker.add_map(alias, config=conf)

    map_info = eventtracker.get_map(map_name)

    assert map_info is not None
    assert map_info['config'] == conf
    assert map_info['creator'] == profile['id']


def test_addmap_profile_not_found(eventtracker):
    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addmap',
        ['not an alias', 'not a title', '{}'],
        f'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_addmap_not_authorized(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addmap',
        [alias, 'not a title', '{}'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_addmap_exists(telosprofile, eventtracker):
    account, alias = telosprofile.new_profile()

    map_name = eventtracker.add_map(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addmap',
        [alias, map_name, '{}'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'map exists' in out
