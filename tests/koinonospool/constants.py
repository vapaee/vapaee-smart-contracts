#!/usr/bin/env python3

import pytest

from pytest_eosiocdt import Asset, Symbol
from pytest_eosiocdt.contract import SmartContract


class KoinonosPool(SmartContract):
    
    @property
    def contract_name(self) -> str:
        return 'koinonospool'

    @property
    def protocol_str(self) -> str:
        return 'koinonos.v1'

    def __init__(self, eosio_testnet):
        super().__init__(eosio_testnet)

    def init_pool(
        self,
        admin: str,
        token_contract: str,
        symbol: Symbol
    ):
        return self.push_action(
            'initpool',
            [admin, token_contract, symbol],
            f'{self.contract_name}@active'
        )

    def get_pools(self):
        return self.get_table(
            self.contract_name, 'pools')

    def get_pools_by_admin(self, admin: str):
        return [
            pool 
            for pool in self.get_pools() 
            if pool['admin'] == admin
        ]

    def fund_pool(
        self,
        creator: str,
        supply: Asset
    ):
        return self.testnet.transfer_token(
            creator,
            self.contract_name,
            supply,
            'fund'
        )

    def convert(
        self,
        sender: str,
        quantity: Asset,
        min_asset: Asset,
        recipient: str
    ):
        return self.testnet.transfer_token(
            sender,
            self.contract_name,
            quantity,
            f'{self.protocol_str},{min_asset},{recipient}'
        )


@pytest.fixture(scope="session")
def koinonospool(eosio_testnet):
    contract = KoinonosPool(eosio_testnet)
    yield contract
