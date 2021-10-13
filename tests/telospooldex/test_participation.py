#!/usr/bin/env python3

import json
import random
import logging

from pytest_eosiocdt import (
    Asset, Symbol, asset_from_str, find_in_balances)

from .constants import telospooldex
from ..telosbookdex.constants import telosbookdex


PART_SYM = Symbol("PART", 8)
PART_UNIT = Asset(1, PART_SYM)

def test_participation_initial(telospooldex, telosbookdex):
    """Create test pool, check pool creator has total participation, then take
    participation back and check tokens get returned.
    """
    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_comm_prec, max_comm_prec = (4, 8)
    min_curr_prec, max_curr_prec = (4, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm) 
    currency_reserve_amount = random.randint(min_curr, max_curr) 

    commodity_supply = commodity_reserve_amount
    currency_supply = currency_reserve_amount

    pool_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply,
        commodity_reserve_amount,
        comm_prec,
        currency_supply,
        currency_reserve_amount,
        curr_prec);

    pool = telospooldex.get_pool(pool_id)
    assert pool

    target_commodity = asset_from_str(pool['commodity_reserve'])
    target_currency = asset_from_str(pool['currency_reserve'])

    comm_sym = target_commodity.symbol
    curr_sym = target_currency.symbol

    assert asset_from_str(
        pool['total_participation']) == PART_UNIT

    assert telospooldex.get_participation(
        pool_id, pool_creator) == PART_UNIT

    ec, _ = telospooldex.withdraw_participation(
        pool_id, pool_creator, PART_UNIT)

    assert ec == 0

    balances = telospooldex.testnet.get_balance(pool_creator)
    assert find_in_balances(balances, comm_sym) == target_commodity 
    assert find_in_balances(balances, curr_sym) == target_currency 
    

def test_participation_multi_towards_zero(telospooldex, telosbookdex):
    """Create test pool, create multiple funders and inside a for loop
    send funds in a way that gives 1/(i * 2) PART score to each funder
    respectively.
    """
    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_comm_prec, max_comm_prec = (4, 8)
    min_curr_prec, max_curr_prec = (4, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    min_prec = min(comm_prec, curr_prec)
    max_err = 1 / (10 ** min_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm) 
    currency_reserve_amount = random.randint(min_curr, max_curr) 

    commodity_supply = commodity_reserve_amount * 3 
    currency_supply = currency_reserve_amount * 3

    market_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply,
        commodity_reserve_amount,
        comm_prec,
        currency_supply,
        currency_reserve_amount,
        curr_prec);

    pool = telospooldex.get_pool(market_id)
    assert pool

    commodity_init_reserve = asset_from_str(pool['commodity_reserve'])
    currency_init_reserve = asset_from_str(pool['currency_reserve'])

    assert commodity_init_reserve.amount == commodity_reserve_amount
    assert currency_init_reserve.amount == currency_reserve_amount

    for i in range(1, 5):
        comm_fund = Asset(commodity_reserve_amount / (i * 2), commodity_init_reserve.symbol)
        curr_fund = Asset(currency_reserve_amount / (i * 2), currency_init_reserve.symbol)

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

        telospooldex.testnet.wait_blocks(10)

        target_participation = Asset(
            float("{:.8f}".format(1 / (i * 2))),
            PART_SYM
        )

        # check participation score
        assert abs(target_participation.amount - telospooldex.get_participation(
            market_id, funder).amount) <= max_err


def test_participation_multi_towards_infinity(telospooldex, telosbookdex):
    """Create test pool, create multiple funders and inside a for loop
    send funds in a way that gives i PART score to each funder
    respectively.
    """
    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_comm_prec, max_comm_prec = (4, 8)
    min_curr_prec, max_curr_prec = (4, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    min_prec = min(comm_prec, curr_prec)
    max_err = 1 / (10 ** min_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm) 
    currency_reserve_amount = random.randint(min_curr, max_curr) 

    commodity_supply = commodity_reserve_amount * 100
    currency_supply = currency_reserve_amount * 100

    market_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply,
        commodity_reserve_amount,
        comm_prec,
        currency_supply,
        currency_reserve_amount,
        curr_prec);

    pool = telospooldex.get_pool(market_id)
    assert pool

    commodity_init_reserve = asset_from_str(pool['commodity_reserve'])
    currency_init_reserve = asset_from_str(pool['currency_reserve'])

    assert commodity_init_reserve.amount == commodity_reserve_amount
    assert currency_init_reserve.amount == currency_reserve_amount

    for i in range(1, 5):
        comm_fund = Asset(commodity_reserve_amount * i, commodity_init_reserve.symbol)
        curr_fund = Asset(currency_reserve_amount * i, currency_init_reserve.symbol)

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

        telospooldex.testnet.wait_blocks(10)

        target_participation = Asset(
            float("{:.8f}".format(i)),
            PART_SYM
        )

        # check participation score
        assert abs(target_participation.amount - telospooldex.get_participation(
            market_id, funder).amount) <= max_err


def test_participation_take_returns(telospooldex, telosbookdex):
    """Create test pool, and fund it, giving total participation to its creator
    then, do a big conversion, finally make creator remove half of its
    participation and check pool reserves get updated accordingly.
    """
    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_comm_prec, max_comm_prec = (4, 8)
    min_curr_prec, max_curr_prec = (4, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm) 
    currency_reserve_amount = random.randint(min_curr, max_curr) 

    commodity_supply = commodity_reserve_amount * 2
    currency_supply = currency_reserve_amount * 2

    pool_id, pool_creator = telospooldex.init_test_pool(
        telosbookdex,
        commodity_supply,
        commodity_reserve_amount,
        comm_prec,
        currency_supply,
        currency_reserve_amount,
        curr_prec);

    pool = telospooldex.get_pool(pool_id)
    assert pool

    commodity_reserve = asset_from_str(pool['commodity_reserve'])
    currency_reserve = asset_from_str(pool['currency_reserve'])

    user = telospooldex.testnet.new_account()
    recipient = telospooldex.testnet.new_account()

    quantity = commodity_reserve

    fee = Asset(
        telospooldex.fee.amount * quantity.amount, quantity.symbol)

    rate = currency_reserve.amount / (
        commodity_reserve.amount + quantity.amount)

    total = Asset(
        rate * (quantity.amount - fee.amount), currency_reserve.symbol)

    _min = Asset(total.amount - 0.01, total.symbol)

    # send all tokens to user to clear creator balances
    balances = telospooldex.testnet.get_balance(pool_creator)
    comm_balance = find_in_balances(balances, commodity_reserve.symbol)
    curr_balance = find_in_balances(balances, currency_reserve.symbol)

    telospooldex.testnet.transfer_token(
        pool_creator, user, comm_balance, '')
    telospooldex.testnet.transfer_token(
        pool_creator, user, curr_balance, '')

    ec, _ = telospooldex.convert(
        user,
        quantity,
        [f'{telospooldex.contract_name}/{currency_reserve.symbol.code}'],
        _min,
        recipient
    )
    assert ec == 0

    part_score = Asset(0.5, PART_SYM)

    ec, _ = telospooldex.withdraw_participation(
        pool_id, pool_creator, part_score)

    assert ec == 0

    # calculate returns
    expected_comm = Asset(
        commodity_reserve.amount + quantity.amount, commodity_reserve.symbol)
    expected_comm.amount *= part_score.amount

    expected_curr = Asset(
        currency_reserve.amount - total.amount, currency_reserve.symbol)
    expected_curr.amount *= part_score.amount

    balances = telospooldex.testnet.get_balance(pool_creator)
    comm_balance = find_in_balances(balances, commodity_reserve.symbol)
    curr_balance = find_in_balances(balances, currency_reserve.symbol)

    assert abs(comm_balance.amount - expected_comm.amount) <= 0.01
    assert abs(curr_balance.amount - expected_curr.amount) <= 0.01
