#!/usr/bin/env python3

import random

from pytest_eosiocdt import Asset, Symbol, asset_from_str
from pytest_eosiocdt.sugar import find_in_balances
from pytest_eosiocdt.telos import telos_token

from .constants import telospooldex
from ..telosbookdex.constants import telosbookdex


SYNC_WAIT = 10


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

    telospooldex.testnet.give_token(
            pool_creator, currency_asset)

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

    telospooldex.testnet.wait_blocks(SYNC_WAIT)

    # fund attempt entry should be deleted
    attempt = telospooldex.get_funding_attempt(market_id)
    assert attempt is None

    pool = telospooldex.get_pool(market_id)
    assert pool
    assert asset_from_str(pool['commodity_reserve']) == commodity_asset
    assert asset_from_str(pool['currency_reserve']) == currency_asset

    # history checks
    history = telospooldex.get_funding_history(market_id, funder)
    assert len(history) == 2

    assert asset_from_str(history[0]['quantity']) == commodity_asset
    assert asset_from_str(history[1]['quantity']) == currency_asset


def test_fund_pool_exact_by_symbol(telosbookdex, telospooldex):

    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_splus, max_splus = (100, 1000)

    min_comm_prec, max_comm_prec = (2, 8)
    min_curr_prec, max_curr_prec = (2, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    max_prec = max(comm_prec, curr_prec) - 1
    min_err = 1 / (10 ** max_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm)
    currency_reserve_amount = random.randint(min_curr, max_curr)

    commodity_supply = commodity_reserve_amount * 2
    currency_supply = currency_reserve_amount * 2 

    market_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply,
        commodity_reserve_amount,
        comm_prec,
        currency_supply,
        currency_reserve_amount,
        curr_prec);

    # target pool reserves after fund
    pool = telospooldex.get_pool(market_id)
    assert pool

    target_reserve_comm = asset_from_str(pool['commodity_reserve'])
    target_reserve_comm.amount *= 2
    target_reserve_curr = asset_from_str(pool['currency_reserve'])
    target_reserve_curr.amount *= 2

    comm_fund = Asset(commodity_reserve_amount, target_reserve_comm.symbol)
    curr_fund = Asset(currency_reserve_amount, target_reserve_curr.symbol)

    market_sym_id = f"{comm_fund.symbol.code}/{curr_fund.symbol.code}"

    # try regular fund
    funder = telospooldex.testnet.new_account()

    telospooldex.testnet.transfer_token(
        pool_creator, funder, comm_fund, '')
    telospooldex.testnet.transfer_token(
        pool_creator, funder, curr_fund, '')

    # send commodity funds
    ec, _ = telospooldex.send_funds(funder, comm_fund, market_sym_id)
    assert ec == 0

    # send currency funds
    ec, _ = telospooldex.send_funds(funder, curr_fund, market_sym_id)
    assert ec == 0

    telospooldex.testnet.wait_blocks(SYNC_WAIT)    

    # fund attempt entry should be deleted
    attempt = telospooldex.get_funding_attempt(market_id)
    assert attempt is None

    pool = telospooldex.get_pool(market_id)
    assert pool

    target_ratio = target_reserve_comm.amount / target_reserve_curr.amount
    ratio = asset_from_str(pool['commodity_reserve']).amount / asset_from_str(pool['currency_reserve']).amount
    err = abs(ratio - target_ratio)
    assert err <= min_err


def test_fund_pool_surplus_commodity(telosbookdex, telospooldex):

    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_splus, max_splus = (100, 1000)

    min_comm_prec, max_comm_prec = (2, 8)
    min_curr_prec, max_curr_prec = (2, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    max_prec = max(comm_prec, curr_prec) - 1
    min_err = 1 / (10 ** max_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm) 
    currency_reserve_amount = random.randint(min_curr, max_curr) 

    surplus = random.randint(min_splus, max_splus)

    commodity_supply = (commodity_reserve_amount * 2) + surplus
    currency_supply = currency_reserve_amount * 2 

    market_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply,
        commodity_reserve_amount,
        comm_prec,
        currency_supply,
        currency_reserve_amount,
        curr_prec);

    # target pool reserves after fund
    pool = telospooldex.get_pool(market_id)
    assert pool

    target_reserve_comm = asset_from_str(pool['commodity_reserve'])
    target_reserve_comm.amount *= 2
    target_reserve_curr = asset_from_str(pool['currency_reserve'])
    target_reserve_curr.amount *= 2

    comm_fund = Asset(commodity_reserve_amount + surplus, target_reserve_comm.symbol)
    curr_fund = Asset(currency_reserve_amount, target_reserve_curr.symbol)

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

    telospooldex.testnet.wait_blocks(SYNC_WAIT)

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
    history = telospooldex.get_funding_history(market_id, funder)
    assert len(history) == 2

    assert asset_from_str(history[0]['quantity']) == comm_fund
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

    max_prec = max(comm_prec, curr_prec) - 1
    min_err = 1 / (10 ** max_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm) 
    currency_reserve_amount = random.randint(min_curr, max_curr) 

    surplus = random.randint(min_splus, max_splus)

    commodity_supply = commodity_reserve_amount * 2
    currency_supply = (currency_reserve_amount * 2) + surplus

    market_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply,
        commodity_reserve_amount,
        comm_prec,
        currency_supply,
        currency_reserve_amount,
        curr_prec);

    # target pool reserves after fund
    pool = telospooldex.get_pool(market_id)
    assert pool

    target_reserve_comm = asset_from_str(pool['commodity_reserve'])
    target_reserve_comm.amount *= 2
    target_reserve_curr = asset_from_str(pool['currency_reserve'])
    target_reserve_curr.amount *= 2

    comm_fund = Asset(commodity_reserve_amount, target_reserve_comm.symbol)
    curr_fund = Asset(currency_reserve_amount + surplus, target_reserve_curr.symbol)

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

    telospooldex.testnet.wait_blocks(SYNC_WAIT)

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
    history = telospooldex.get_funding_history(market_id, funder)
    assert len(history) == 2

    assert asset_from_str(history[0]['quantity']) == comm_fund
    assert asset_from_str(history[1]['quantity']) == curr_fund
