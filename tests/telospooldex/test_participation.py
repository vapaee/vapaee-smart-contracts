#!/usr/bin/env python3

import random

from pytest_eosiocdt import Asset, Symbol, asset_from_str

from .constants import telospooldex
from ..telosbookdex.constants import telosbookdex


PART_SYM = Symbol("PART", 8)
PART_UNIT = Asset(1, PART_SYM)

def test_participation_initial(telospooldex, telosbookdex):
    """Create test pool, check pool creator has total participation.
    """

    pool_id, pool_creator = telospooldex.init_test_pool(telosbookdex)

    pool = telospooldex.get_pool(pool_id)
    assert pool

    assert asset_from_str(
        pool['total_participation']) == PART_UNIT

    assert telospooldex.get_participation(
        pool_id, pool_creator) == PART_UNIT


def test_participation_multi(telospooldex, telosbookdex):
    """Create test pool, create multiple funders and check their respective
    pariticipation scores.
    """
    # test parameters
    min_comm, max_comm = (10000, 100000)
    min_curr, max_curr = (10000, 100000)
    
    min_comm_prec, max_comm_prec = (2, 8)
    min_curr_prec, max_curr_prec = (2, 8)

    comm_prec = random.randint(min_comm_prec, max_comm_prec)
    curr_prec = random.randint(min_curr_prec, max_curr_prec)

    commodity_reserve_amount = random.randint(min_comm, max_comm) 
    currency_reserve_amount = random.randint(min_curr, max_curr) 

    commodity_supply = commodity_reserve_amount * 10
    currency_supply = currency_reserve_amount * 10

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
            market_id, funder).amount) <= (1 / (10 ** 7))
