#!/usr/bin/env python3

from datetime import datetime

from .constants import EventTracker, eventtracker
from ..telosprofile.constants import telosprofile


def test_addseason(telosprofile, eventtracker):
    """Create profile & add season using it, check tables
    """
    account, alias = telosprofile.new_profile()
    profile = telosprofile.get_profile(alias)

    conf = '{\'test\': true}'
    season_name = eventtracker.add_season(alias, config=conf)

    season_info = eventtracker.get_season(season_name)

    assert season_info is not None
    assert season_info['config'] == conf

    assert profile['id'] in season_info['tc_whitelist']


def test_addseason_profile_not_found(eventtracker):
    """Attempt to add a season using a non existent profile, check error message
    """
    date = (datetime.fromtimestamp(0)).isoformat()
    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addseason',
        ['not an alias', 'not a title', date, date, '{}'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_addseason_not_authorized(telosprofile, eventtracker):
    """Attempt to add a season using the wrong signature, check error message
    """
    account, alias = telosprofile.new_profile()
    date = (datetime.fromtimestamp(0)).isoformat()

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addseason',
        [alias, 'not a title', date, date, '{}'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_addseason_exists(telosprofile, eventtracker):
    """Attempt to add a season with a name already in use, check error message
    """
    account, alias = telosprofile.new_profile()
    date = (datetime.fromtimestamp(0)).isoformat()

    season_name = eventtracker.add_season(alias)

    ec, out = eventtracker.testnet.push_action(
        EventTracker.contract_name,
        'addseason',
        [alias, season_name, date, date, '{}'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'season exists' in out
