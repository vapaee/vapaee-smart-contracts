#!/usr/bin/env python3

import pytest

from pytest_eosiocdt import (
    Asset,
    Symbol,
    collect_stdout
)
from pytest_eosiocdt.contract import SmartContract


class TelosPoolDEX(SmartContract):

    @property
    def contract_name(self) -> str:
        return 'telospooldex'

    def __init__(self, eosio_testnet):
        super().__init__(eosio_testnet)

    def get_pools(self):
        return self.get_table(self.contract_name, 'pools')

    def get_pool(self, pool_id: int):
        pools = self.get_pools()
        return next((
            pool for pool in pools
            if pool['id'] == pool_id),
            None
        )

    def get_funding_attempts(self):
        return self.get_table(self.contract_name, 'fundattempts')

    def get_funding_attempt(self, attempt_id: int):
        attempts = self.get_funding_attempts()
        return next((
            attempt for attempt in attempts
            if attempt['id'] == attempt_id),
            None
        )

    def init_pool(
        self,
        creator: str,
        market_id: int
    ):
        return self.push_action(
            'initpool',
            [creator, market_id],
            f'{creator}@active'
        )

    def try_fund(
        self,
        funder: str,
        market_id: int,
        commodity: Symbol,
        currency: Symbol
    ):
        ec, out = self.push_action(
            'tryfund',
            [funder, market_id, commodity, currency],
            f'{funder}@active'
        )
        if ec == 0:
            out = int(collect_stdout(out).rstrip())
        return ec, out

    def direct_fund(
        self,
        funder: str,
        quantity: Asset,
        pool_id: int
    ):
        return self.testnet.transfer_token(
            funder,
            self.contract_name,
            quantity,
            f'directfund,{pool_id}'
        )

    def send_funds(
        self,
        funder: str,
        quantity: Asset,
        attempt_id: int
    ):
        return self.testnet.transfer_token(
            funder,
            self.contract_name,
            quantity,
            f'fund,{attempt_id}'
        )

    def cancel_fund(
        self,
        funder: str,
        attempt_id: int
    ):
        return self.push_action(
            'cancelfund',
            [funder, attempt_id],
            f'{funder}@active'
        )


@pytest.fixture(scope="session")
def telospooldex(eosio_testnet):
    contract = TelosPoolDEX(eosio_testnet)
    yield contract

