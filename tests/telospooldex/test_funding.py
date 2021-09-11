#!/usr/bin/env python3

import random
import logging

from pytest_eosiocdt import Asset, Symbol, asset_from_str
from pytest_eosiocdt.sugar import find_in_balances
from pytest_eosiocdt.telos import telos_token

from .constants import telospooldex
from ..telosbookdex.constants import telosbookdex


def test_fund_pool_exact(telosbookdex, telospooldex):
    """Create test token and a market on telosbookdex pairing it with telos_token,
    then use direct fund to get the pool to a ratio of 0.25, then attempt a normal
    fund using an exact ratio.

    At the end check fund attempt history got updated correctly.
    """

    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)

    commodity_supply = random.randint(min_comm, max_comm)
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

    # direct fund
    commodity_amount = (commodity_supply / 2) - 1
    currency_amount = random.randint(min_curr, max_curr)

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

    # send commodity funds
    ec, _ = telospooldex.send_funds(funder, commodity_asset, market_id)
    assert ec == 0

    # send currency funds
    ec, _ = telospooldex.send_funds(funder, currency_asset, market_id)
    assert ec == 0

    # fund attempt entry should be deleted
    attempt = telospooldex.get_funding_attempt(market_id)
    assert attempt is None

    # pool should have the same ratio but double the liquidity
    commodity_total = Asset(commodity_asset.amount * 2, test_token)
    currency_total = Asset(currency_asset.amount * 2, telos_token)

    pool = telospooldex.get_pool(market_id)
    assert pool
    assert asset_from_str(pool['commodity_reserve']) == commodity_total
    assert asset_from_str(pool['currency_reserve']) == currency_total

    # history checks
    history = telospooldex.get_funding_history(funder)
    assert len(history) == 2

    assert history[0]['pool_id'] == market_id
    assert asset_from_str(history[0]['quantity']) == commodity_asset

    assert history[1]['pool_id'] == market_id
    assert asset_from_str(history[1]['quantity']) == currency_asset


def test_fund_pool_surplus_commodity(telosbookdex, telospooldex):

    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_splus, max_splus = (100, 1000)

    min_comm_prec, max_comm_prec = (2, 8)
    min_curr_prec, max_curr_prec = (2, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    max_prec = max(comm_prec, curr_prec)
    min_err = 1 / (10 ** max_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm)
    currency_reserve_amount = random.randint(min_curr, max_curr)

    surplus = random.randint(min_splus, max_splus)

    commodity_supply = (commodity_reserve_amount * 2) + surplus
    currency_supply = currency_reserve_amount * 2 

    market_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply + 2,
        commodity_reserve_amount,
        comm_prec,
        currency_supply + 2,
        currency_reserve_amount,
        curr_prec);

    pool = telospooldex.get_pool(market_id)
    assert pool

    # target pool reserves after fund
    target_reserve_comm = asset_from_str(pool['commodity_reserve'])
    target_reserve_comm.amount *= 2
    target_reserve_curr = asset_from_str(pool['currency_reserve'])
    target_reserve_curr.amount *= 2

    # init fund assets
    comm_fund = asset_from_str(pool['commodity_reserve'])
    comm_fund.amount += surplus

    curr_fund = asset_from_str(pool['currency_reserve'])

    # try regular fund
    funder = telospooldex.testnet.new_account()

    telospooldex.testnet.transfer_token(
        pool_creator, funder, comm_fund, '')
    telospooldex.testnet.transfer_token(
        pool_creator, funder, curr_fund, '')

    # send commodity funds
    ec, _ = telospooldex.send_funds(funder, comm_fund, market_id)
    assert ec == 0

    # send currency funds
    ec, _ = telospooldex.send_funds(funder, curr_fund, market_id)
    assert ec == 0

    # fund attempt entry should be deleted
    attempt = telospooldex.get_funding_attempt(market_id)
    assert attempt is None

    pool = telospooldex.get_pool(market_id)
    assert pool

    target_ratio = target_reserve_comm.amount / target_reserve_curr.amount
    ratio = asset_from_str(pool['commodity_reserve']).amount / asset_from_str(pool['currency_reserve']).amount
    err = abs(ratio - target_ratio)
    assert err <= min_err

    # pool should have returned the surplus sent
    funder_balances = telospooldex.testnet.get_balance(funder)
    actual_surplus = find_in_balances(funder_balances, comm_fund.symbol)

    err = abs(actual_surplus.amount - surplus)
    assert err <= 0.01 

    # history checks
    history = telospooldex.get_funding_history(funder)
    assert len(history) == 2

    assert history[0]['pool_id'] == market_id
    assert asset_from_str(history[0]['quantity']) == comm_fund

    assert history[1]['pool_id'] == market_id
    assert asset_from_str(history[1]['quantity']) == curr_fund


def test_fund_pool_surplus_currency(telosbookdex, telospooldex):

    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_splus, max_splus = (100, 1000)

    min_comm_prec, max_comm_prec = (2, 8)
    min_curr_prec, max_curr_prec = (2, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    max_prec = max(comm_prec, curr_prec)
    min_err = 1 / (10 ** max_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm)
    currency_reserve_amount = random.randint(min_curr, max_curr)

    surplus = random.randint(min_splus, max_splus)

    commodity_supply = commodity_reserve_amount * 2
    currency_supply = (currency_reserve_amount * 2) + surplus

    market_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply + 2,
        commodity_reserve_amount,
        comm_prec,
        currency_supply + 2,
        currency_reserve_amount,
        curr_prec);

    pool = telospooldex.get_pool(market_id)
    assert pool

    # target pool reserves after fund
    target_reserve_comm = asset_from_str(pool['commodity_reserve'])
    target_reserve_comm.amount *= 2
    target_reserve_curr = asset_from_str(pool['currency_reserve'])
    target_reserve_curr.amount *= 2

    # init fund assets
    comm_fund = asset_from_str(pool['commodity_reserve'])

    curr_fund = asset_from_str(pool['currency_reserve'])
    curr_fund.amount += surplus

    # try regular fund
    funder = telospooldex.testnet.new_account()

    telospooldex.testnet.transfer_token(
        pool_creator, funder, comm_fund, '')
    telospooldex.testnet.transfer_token(
        pool_creator, funder, curr_fund, '')

    # send commodity funds
    ec, _ = telospooldex.send_funds(funder, comm_fund, market_id)
    assert ec == 0

    # send currency funds
    ec, _ = telospooldex.send_funds(funder, curr_fund, market_id)
    assert ec == 0

    # fund attempt entry should be deleted
    attempt = telospooldex.get_funding_attempt(market_id)
    assert attempt is None

    pool = telospooldex.get_pool(market_id)
    assert pool

    target_ratio = target_reserve_comm.amount / target_reserve_curr.amount
    ratio = asset_from_str(pool['commodity_reserve']).amount / asset_from_str(pool['currency_reserve']).amount
    err = abs(ratio - target_ratio)
    assert err <= min_err

    # pool should have returned the surplus sent
    funder_balances = telospooldex.testnet.get_balance(funder)
    actual_surplus = find_in_balances(funder_balances, curr_fund.symbol)

    err = abs(actual_surplus.amount - surplus)
    assert err <= 0.01 

    # history checks
    history = telospooldex.get_funding_history(funder)
    assert len(history) == 2

    assert history[0]['pool_id'] == market_id
    assert asset_from_str(history[0]['quantity']) == comm_fund

    assert history[1]['pool_id'] == market_id
    assert asset_from_str(history[1]['quantity']) == curr_fund
