#!/usr/bin/env python3

import json
import string
import random

from typing import Optional
from datetime import datetime

import pytest

from ..telosprofile.constants import TelosProfile


class EventTracker:
    contract_name = 'eventtracker'

    def __init__(self, eosio_testnet):
        self.testnet = eosio_testnet
        self.telosprofile = TelosProfile(eosio_testnet)

    def get_map(self, title: str):
        maps = self.testnet.get_table(
            EventTracker.contract_name,
            EventTracker.contract_name,
            'maps'
        )

        return next((
            row for row in maps
            if row['title'] == title),
            None
        )

    def add_map(
        self,
        alias: str,
        title: Optional[str] = None,
        config: Optional[str] = None
    ):
        profile = self.telosprofile.get_profile(alias)
        assert profile is not None

        if title is None:
            title = ''.join(
                random.choice(string.ascii_lowercase + string.digits)
                for _ in range(256)
            )

        if config is None:
            config = '{}'

        ec, out = self.testnet.push_action(
            EventTracker.contract_name,
            'addmap',
            [alias, title, config],
            f'{profile["owners"][0]}@active'
        )

        assert ec == 0

        return title 

    def get_season(self, title: str):
        seasons = self.testnet.get_table(
            EventTracker.contract_name,
            EventTracker.contract_name,
            'seasons'
        )

        return next((
            row for row in seasons
            if row['title'] == title),
            None
        )

    def add_season(
        self,
        alias: str,
        title: Optional[str] = None,
        start: Optional[str] = None,
        finish: Optional[str] = None,
        config: Optional[str] = None
    ):
        profile = self.telosprofile.get_profile(alias)
        assert profile is not None

        if title is None:
            title = ''.join(
                random.choice(string.ascii_lowercase + string.digits)
                for _ in range(256)
            )

        if start is None:
            start = (datetime.now()).isoformat()

        if finish is None:
            finish = (datetime.fromtimestamp(0)).isoformat()

        if config is None:
            config = '{}'

        ec, out = self.testnet.push_action(
            EventTracker.contract_name,
            'addseason',
            [alias, title, start, finish, config],
            f'{profile["owners"][0]}@active'
        )

        assert ec == 0

        return title

    def add_target_creator(
        self,
        alias: str,
        season: str,
        new_alias: str
    ):
        profile = self.telosprofile.get_profile(alias)
        assert profile is not None

        ec, out = self.testnet.push_action(
            EventTracker.contract_name,
            'addtcwlist',
            [alias, season, new_alias],
            f'{profile["owners"][0]}@active'
        )

        return ec, out

    def get_target(self, season: str, title: str):
        season_info = self.get_season(season)
        targets = self.testnet.get_table(
            EventTracker.contract_name,
            str(season_info['id']),
            'targets'
        )

        return next((
            row for row in targets
            if row['title'] == title),
            None
        )


    def add_target(
        self,
        alias: str,
        season: str,
        map_: str,
        title: Optional[str] = None,
        config: Optional[str] = None,
        lat: Optional[str] = None,
        lng: Optional[str] = None
    ):
        profile = self.telosprofile.get_profile(alias)
        assert profile is not None

        if title is None:
            title = ''.join(
                random.choice(string.ascii_lowercase + string.digits)
                for _ in range(256)
            )

        if config is None:
            config = '{}'

        if lat is None:
            lat = '0 LAT'

        if lng is None:
            lng = '0 LONG'

        ec, out = self.testnet.push_action(
            EventTracker.contract_name,
            'addtarget',
            [alias, season, title, config, map_, lat, lng],
            f'{profile["owners"][0]}@active'
        )

        assert ec == 0

        return title

    def add_to_target_whitelist(
        self,
        alias: str,
        season: str,
        target: str,
        new_alias: str
    ):
        profile = self.telosprofile.get_profile(alias)
        assert profile is not None

        ec, out = self.testnet.push_action(
            EventTracker.contract_name,
            'addwlist',
            [alias, season, target, new_alias],
            f'{profile["owners"][0]}@active'
        )

        return ec, out

    def get_events_with_config(
        self,
        season: str,
        target: str,
        **kwargs
    ):
        season_info = self.get_season(season)
        target_info = self.get_target(season, target)
        events = self.testnet.get_table(
            EventTracker.contract_name,
            str(season_info['id']),
            'events'
        )

        results = []
        for row in events:
            if row['target'] == target_info['id']:
                cfg = json.loads(row['config'])
                is_equal = True
                for kw, arg in kwargs.items():
                    is_equal &= cfg[kw] == arg
            
                if is_equal:
                    results.append(row)

        return results


    def add_event(
        self,
        alias: str,
        season: str,
        target: str,
        map_: str,
        lat: Optional[str] = None,
        lng: Optional[str] = None,
        geometry: Optional[str] = None,
        config: Optional[str] = None
    ):
        profile = self.telosprofile.get_profile(alias)
        assert profile is not None

        if geometry is None:
            geometry = '{}'

        if config is None:
            config = '{}'

        if lat is None:
            lat = '0 LAT'

        if lng is None:
            lat = '0 LONG'

        ec, out = self.testnet.push_action(
            EventTracker.contract_name,
            'addevent',
            [
                alias,
                season,
                target,
                map_,
                lat,
                lng,
                geometry,
                config
            ],
            f'{profile["owners"][0]}@active'
        )

        return ec, out


@pytest.fixture(scope="session")
def eventtracker(eosio_testnet):
    contract = EventTracker(eosio_testnet)
    yield contract

