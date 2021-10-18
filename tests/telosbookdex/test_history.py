#!/usr/bin/env python3

from time import time

from pytest_eosio import (
    random_token_symbol,
    collect_stdout,
    name_to_string
)

from .constants import telosbookdex


def test_order_check_history(telosbookdex):
    """Perform a order execution and check if all history tables are
    updated (``history``, ``historyall``, ``historyblock`` & ``events``).
    """
    # setup token & seller account
    supply = 1000
    symbol, precision, seller, seller_id = telosbookdex.init_test_token(
        max_supply=supply,
        precision=8
    )
    
    # generate needed eosio asset strings
    amount = 300
    price = 1000
    total = amount * price

    str_amount = format(amount, f'.{precision}f')
    str_asset_amount = f'{str_amount} {symbol}'
    str_asset_price = f'{price:.4f} TLOS'
    str_asset_total = f'{total:.4f} TLOS'

    # setup buyer account
    buyer = telosbookdex.testnet.new_account()
    ec, buyer_id = telosbookdex.new_client(admin=buyer)
    assert ec == 0
    telosbookdex.testnet.transfer_token(
        'eosio.token',
        buyer,
        str_asset_total,
        ''
    )
    telosbookdex.deposit(
        buyer,
        str_asset_total
    )

    # place sell order
    ec, out = telosbookdex.place_order(
        seller,
        'sell',
        str_asset_amount,
        str_asset_price,
        seller_id
    )

    assert ec == 0

    # place buy order
    ec, out = telosbookdex.place_order(
        buyer,
        'buy',
        str_asset_amount,
        str_asset_price,
        buyer_id
    )

    assert ec == 0
    
    # history logs individual transaction events, market scope
    market_id, history = telosbookdex.get_history(symbol, 'TLOS')

    assert len(history) == 1

    trade = history[0]

    assert trade['buyer'] == buyer
    assert trade['seller'] == seller
    assert trade['price'] == str_asset_price
    assert trade['amount'] == str_asset_amount
    assert trade['payment'] == str_asset_total

    # historyall indexes separate market history table events in a global
    # single table
    history_all = telosbookdex.get_history_all()
   
    matches = [
        event
        for event in history_all
        if trade['id'] == event['key']
    ]
    assert len(matches) == 1

    hall_trade = matches[0]

    assert name_to_string(hall_trade['market']) == market_id
    assert hall_trade['date'] == trade['date']

    market = telosbookdex.get_market_by_id(hall_trade['market'])
    assert market

    # events logs individual events of all kinds, not only transactions
    # global scope
    event_params = '|'.join([
        f'{market["currency"]}/{market["commodity"]}',
        buyer,
        seller,
        str_asset_amount,
        str_asset_total,
        str_asset_price
    ])

    matches = [
        event
        for event in telosbookdex.get_events()
        if ((event['user'] == buyer) and
            (event['event'] == 'transaction') and
            (event['params'] == event_params) and
            (event['date'] == trade['date']))
    ]

    assert len(matches) == 1

    # historyblocks stores information about market status in an hour by hour
    # basis
    hblocks = telosbookdex.get_history_blocks(market_id)

    assert len(hblocks) == 1

    hblock = hblocks[0]

    assert hblock['hour'] == int(time() / 3600)
    assert hblock['price'] == str_asset_price
    assert hblock['volume'] == str_asset_total
