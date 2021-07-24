#!/usr/bin/env python3

from pytest_eosiocdt import Asset, Symbol, asset_from_str
from pytest_eosiocdt.telos import telos_token

from .constants import telospooldex
from ..telosbookdex.constants import telosbookdex


def test_fund_pool(telosbookdex, telospooldex):
    commodity_supply = 10000
    symbol, precision, seller, seller_id = telosbookdex.init_test_token(
        max_supply=commodity_supply,
        precision=4
    )

    # place dummy order to create markets
    test_token = Symbol(symbol, precision)
    dummy_comm = Asset(1, test_token)
    dummy_curr = Asset(1, telos_token) 
    ec, _ = telosbookdex.place_order(
            seller, 'sell', dummy_comm, dummy_curr, seller_id)
    assert ec == 0

    market = telosbookdex.get_market(test_token.code, telos_token.code)
    assert market

    pool_creator = telospooldex.testnet.new_account()
    market_id = market['id']

    # create pool
    ec, _ = telospooldex.init_pool(pool_creator, market_id)
    assert ec == 0

    pool = telospooldex.get_pool(market_id)
    assert pool

    # direct fund
    commodity_amount = 500
    currency_amount = 2000

    commodity_asset = Asset(commodity_amount, test_token)
    currency_asset = Asset(currency_amount, telos_token)

    telosbookdex.withdraw(seller, commodity_asset, seller_id)
    telospooldex.testnet.transfer_token(
        seller, pool_creator, commodity_asset, '')

    ec, _ = telospooldex.direct_fund(
        pool_creator, commodity_asset, market_id)
    assert ec == 0

    telospooldex.testnet.give_token(
            pool_creator, currency_asset)

    ec, _ = telospooldex.direct_fund(
        pool_creator, currency_asset, market_id)
    assert ec == 0

    # at this point pool should be funded with a ratio of
    # commodity_amount / currency_amount
    pool = telospooldex.get_pool(market_id)
    assert pool
    assert asset_from_str(pool['commodity_reserve']) == commodity_asset
    assert asset_from_str(pool['currency_reserve']) == currency_asset

    # try regular fund
    funder = telospooldex.testnet.new_account()

    # give him assets
    telosbookdex.withdraw(seller, commodity_asset, seller_id)
    telospooldex.testnet.transfer_token(
        seller, funder, commodity_asset, '')

    telospooldex.testnet.give_token(
            funder, currency_asset)

    ec, attempt_id = telospooldex.try_fund(
        funder, market_id, test_token, telos_token)
    assert ec == 0

    # send commodity funds
    ec, _ = telospooldex.send_funds(funder, commodity_asset, attempt_id)
    assert ec == 0

    # send currency funds
    ec, _ = telospooldex.send_funds(funder, currency_asset, attempt_id)
    assert ec == 0

    # fund attempt entry should be deleted
    attempt = telospooldex.get_funding_attempt(attempt_id)
    assert attempt is None

    # pool should have the same ratio but double the liquidity
    commodity_total = Asset(commodity_asset.amount * 2, test_token)
    currency_total = Asset(currency_asset.amount * 2, telos_token)

    pool = telospooldex.get_pool(market_id)
    assert pool
    assert asset_from_str(pool['commodity_reserve']) == commodity_total
    assert asset_from_str(pool['currency_reserve']) == currency_total
