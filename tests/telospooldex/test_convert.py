#!/usr/bin/env python3

import copy
import random
import logging

from pytest_eosiocdt import asset_from_str, Asset, Symbol

from .constants import telospooldex
from ..telosbookdex.constants import telosbookdex


def test_convert_single(telosbookdex, telospooldex):
    """Create a new pool using the telospooldex.init_test_pool function, create
    sender & recipient accounts, create a random quantity asset of the same
    symbol than the pool's commodity reserve, then figure out how much we should
    get total if we convert to the currency reserve symbol.

    Then we withdraw the exact quantity we wanted to convert and send the
    transfer following the openpool.v1 protocol.

    If the transaction works that means we already got what we expected cause 
    the protocol has a minimun accepted field, but we check the pool reserves
    behind the scenes to check everything got updated correctly.
    """

    pool_id, commodity_seller, currency_seller = telospooldex.init_test_pool(telosbookdex)
    
    pool = telospooldex.get_pool(pool_id)
    assert pool

    commodity_reserve = asset_from_str(pool['commodity_reserve'])
    currency_reserve = asset_from_str(pool['currency_reserve'])

    sender = telospooldex.testnet.new_account()
    recipient = telospooldex.testnet.new_account()

    # quantity to convert
    quantity = Asset(random.randint(10, 1000), commodity_reserve.symbol)

    # figure out convertion on our own
    rate = currency_reserve.amount / (
        commodity_reserve.amount + quantity.amount)

    total = Asset(
        rate * quantity.amount, currency_reserve.symbol)

    _min = Asset(total.amount - 0.01, total.symbol)

    # withdraw & transfer exact amount needed to sender
    ec, _ = telosbookdex.withdraw(
        commodity_seller, quantity,
        telosbookdex.get_client(commodity_seller)['id']
    )
    assert ec == 0
    ec, _ = telospooldex.testnet.transfer_token(
        commodity_seller, sender, quantity, '')
    assert ec == 0

    # finally convert
    ec, _ = telospooldex.convert(
        sender,
        quantity,
        [f'{telospooldex.contract_name}/{currency_reserve.symbol.code}'],
        _min,
        recipient
    )

    assert ec == 0

    pool = telospooldex.get_pool(pool_id)
    assert pool

    assert (asset_from_str(pool['commodity_reserve']).amount -
        (commodity_reserve.amount + quantity.amount) < 0.01)

    assert (asset_from_str(pool['currency_reserve']).amount -
        (currency_reserve.amount - total.amount) < 0.01)


def test_convert_multi(telosbookdex, telospooldex):
    """This test creates tokens registered on the `telosbookdex` contract, then
    links them together in token groups like this:

        tokens: A, B, C, D
        groups: (A, B*), (B, C*), (C, D*)

    Tokens with asterisks get marked as currencies whitin those groups, to create
    those markets we place dummy orders in each of them.

    Then we create a pool for each market in `telospooldex`, and send some tokens
    using the `directfund` mechanism.

    Next, we chose a random quantity of tokens of symbol A and figure out each
    rate and conversion to go from A, to B, to C and finally to D. We craft a 
    valid path string following the protocol and send the conversion.

    Same as the single conversion test we check the starting pool's commodity
    reserves and the ending pool's currency reserves to see if they got updated
    correctly.

    Note: in this test, the amount of tokens/groups/markets/pools/jumps is
    controlled by the ``token_amount`` variable, and must be bigger than 2.

    If the number is bigger than 3, the system works, but it seems eosio max
    action call depth gets reached quickly if we perform many jumps.
    """

    min_precision, max_precision = (4, 8)
    _min_supply, _max_supply = (10000, 99999999)

    # register tokens in telosbookdex
    token_amount = 3
    tokens = []
    for i in range(token_amount):
        precision = random.randint(min_precision, max_precision)
        max_supply = random.randint(_min_supply, _max_supply)
        token_data = telosbookdex.init_test_token(
            max_supply=max_supply,
            precision=precision
        )

        sym, _, seller, seller_id = token_data

        supply = Asset(max_supply, Symbol(sym, precision))

        tokens.append({
            'seller': seller,
            'seller_id': seller_id,
            'supply': supply
        })

    def get_token_info(sym_code: str):
        return next((
            token for token in tokens
            if token['supply'].symbol.code == sym_code),
            None
        )

    # create a chain of groups between tokens
    groups = []
    markets = []
    for i in range(token_amount - 1):
        token_a = tokens[i]
        token_b = tokens[i + 1]

        ec, group_id = telosbookdex.new_token_group(token_b['seller'])
        assert ec == 0

        ec, _ = telosbookdex.change_token_groups(
            token_a['seller'],
            token_a['supply'].symbol.code,
            [group_id]
        )
        assert ec == 0

        ec, _ = telosbookdex.change_token_groups(
            token_b['seller'],
            token_b['supply'].symbol.code,
            [group_id]
        )
        assert ec == 0

        # set token_b as currency for group
        ec, _ = telosbookdex.set_currency(
            token_b['seller'],
            token_b['supply'].symbol.code,
            True,
            group_id
        )
        assert ec == 0

        # place dummy order to create markets
        dummy_comm = Asset(1, token_a['supply'].symbol)
        dummy_curr = Asset(1, token_b['supply'].symbol) 
        ec, _ = telosbookdex.place_order(
            token_a['seller'], 'sell', dummy_comm, dummy_curr, token_a['seller_id'])
        assert ec == 0

        market = telosbookdex.get_market(
            token_a['supply'].symbol.code, token_b['supply'].symbol.code)
        assert market

        markets.append(market)

    pools = []
    for market in markets:
        pool_creator = telospooldex.testnet.new_account()
        market_id = market['id']

        # create pool
        ec, _ = telospooldex.create_pool(pool_creator, market_id)
        assert ec == 0

        pool = telospooldex.get_pool(market_id)
        assert pool

        commodity = get_token_info(market['commodity'])
        currency = get_token_info(market['currency'])

        # direct fund pool

        commodity_reserve = Asset(
            commodity['supply'].amount / 4, commodity['supply'].symbol)

        currency_reserve = Asset(
            currency['supply'].amount / 4, currency['supply'].symbol)

        telosbookdex.withdraw(
            commodity['seller'], commodity_reserve, commodity['seller_id'])
        telospooldex.testnet.transfer_token(
            commodity['seller'], pool_creator, commodity_reserve, '')

        ec, _ = telospooldex.direct_fund(
            pool_creator, commodity_reserve, market_id)
        assert ec == 0

        telosbookdex.withdraw(
            currency['seller'], currency_reserve, currency['seller_id'])
        telospooldex.testnet.transfer_token(
            currency['seller'], pool_creator, currency_reserve, '')

        ec, _ = telospooldex.direct_fund(
            pool_creator, currency_reserve, market_id)
        assert ec == 0

        # save pool data
        pool = telospooldex.get_pool(market_id)
        assert pool

        pool['commodity_seller'] = commodity['seller']
        pool['currency_seller'] = currency['seller']
        pool['commodity_reserve'] = asset_from_str(pool['commodity_reserve'])
        pool['currency_reserve'] = asset_from_str(pool['currency_reserve'])

        pools.append(pool)

    sender = telospooldex.testnet.new_account()
    recipient = telospooldex.testnet.new_account()
    
    # figure out convertion on our own
    starting_quantity = Asset(
        random.randint(10, 1000),
        pools[0]['commodity_reserve'].symbol
    )

    quantity = copy.copy(starting_quantity)

    for i in range(len(pools)):
        rate = pools[i]['currency_reserve'].amount / (
            pools[i]['commodity_reserve'].amount + quantity.amount)

        total = Asset(
            rate * quantity.amount,
            pools[i]['currency_reserve'].symbol
        )

        _min = Asset(total.amount - 0.01, total.symbol)

        quantity = total

    # withdraw & transfer exact amount needed to sender
    ec, _ = telosbookdex.withdraw(
        pools[0]['commodity_seller'], starting_quantity,
        telosbookdex.get_client(
            pools[0]['commodity_seller'])['id']
    )
    assert ec == 0

    ec, _ = telospooldex.testnet.transfer_token(
        pools[0]['commodity_seller'], sender, starting_quantity, '')
    assert ec == 0

    # create path
    path  = [
        f'telospooldex/{pool["currency_reserve"].symbol.code}'
        for pool in pools
    ]

    # finally convert
    ec, _ = telospooldex.convert(
        sender,
        starting_quantity,
        path,
        _min,
        recipient
    )

    assert ec == 0

    starting_pool = telospooldex.get_pool(pools[0]['id'])
    assert starting_pool

    ending_pool = telospooldex.get_pool(pools[-1]['id'])
    assert ending_pool

    assert (asset_from_str(starting_pool['commodity_reserve']).amount -
        (pools[0]['commodity_reserve'].amount + starting_quantity.amount) < 0.01)

    assert (asset_from_str(ending_pool['currency_reserve']).amount -
        (pools[-1]['currency_reserve'].amount - total.amount) < 0.01)

