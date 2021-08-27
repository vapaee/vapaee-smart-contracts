#!/usr/bin/env python3

from typing import List

import pytest

from pytest_eosiocdt import (
    Asset,
    Symbol,
    collect_stdout,
    asset_from_str
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
            'createpool',
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

    def convert(
        self,
        _from: str,
        quantity,
        path: List[str],
        _min: Asset,
        recipient: str
    ):
        return self.testnet.transfer_token(
            _from,
            self.contract_name,
            quantity,
            f'openpool.v1,{" ".join(path)},{_min},{recipient}'
        )

    def init_test_pool(
        self,
        telosbookdex,
        commodity_amount: int = 100000,
        commodity_reserve_amount: int = 50000,
        commodity_precision: int = 4,
        currency_amount: int = 100000,
        currency_reserve_amount: int = 50000,
        currency_precision: int = 4
    ):
        # TODO: this is ugly we need to update our test apis to use the new
        # sugary Asset and Symbol classes.
        token_data = telosbookdex.init_test_token(
            max_supply=commodity_amount,
            precision=commodity_precision
        )

        sym, precision, commodity_seller, commodity_seller_id = token_data

        commodity_supply = Asset(commodity_amount, Symbol(sym, precision))

        token_data = telosbookdex.init_test_token(
            max_supply=currency_amount,
            precision=currency_precision
        )

        sym, precision, currency_seller, currency_seller_id = token_data

        currency_supply = Asset(currency_amount, Symbol(sym, precision))

        # create token group
        ec, group_id = telosbookdex.new_token_group(currency_seller)
        assert ec == 0

        ec, _ = telosbookdex.change_token_groups(
            commodity_seller,
            commodity_supply.symbol.code,
            [group_id]
        )
        assert ec == 0
        ec, _ = telosbookdex.change_token_groups(
            currency_seller,
            currency_supply.symbol.code,
            [group_id]
        )
        assert ec == 0

        # set currency
        ec, _ = telosbookdex.set_currency(
            currency_seller,
            currency_supply.symbol.code,
            True,
            group_id
        )
        assert ec == 0

        # place dummy order to create markets
        dummy_comm = Asset(1, commodity_supply.symbol)
        dummy_curr = Asset(1, currency_supply.symbol) 
        ec, _ = telosbookdex.place_order(
            commodity_seller, 'sell', dummy_comm, dummy_curr, commodity_seller_id)
        assert ec == 0

        market = telosbookdex.get_market(
            commodity_supply.symbol.code, currency_supply.symbol.code)
        assert market

        pool_creator = self.testnet.new_account()
        market_id = market['id']

        # create pool
        ec, _ = self.init_pool(pool_creator, market_id)
        assert ec == 0

        pool = self.get_pool(market_id)
        assert pool

        commodity_asset = Asset(commodity_reserve_amount, commodity_supply.symbol)
        currency_asset = Asset(currency_reserve_amount, currency_supply.symbol)

        telosbookdex.withdraw(
            commodity_seller, commodity_asset, commodity_seller_id)
        self.testnet.transfer_token(
            commodity_seller, pool_creator, commodity_asset, '')

        ec, _ = self.direct_fund(
            pool_creator, commodity_asset, market_id)
        assert ec == 0

        telosbookdex.withdraw(
            currency_seller, currency_asset, currency_seller_id)
        self.testnet.transfer_token(
            currency_seller, pool_creator, currency_asset, '')

        ec, _ = self.direct_fund(
            pool_creator, currency_asset, market_id)
        assert ec == 0

        return market_id, commodity_seller, currency_seller


@pytest.fixture(scope="session")
def telospooldex(eosio_testnet):
    contract = TelosPoolDEX(eosio_testnet)
    yield contract
