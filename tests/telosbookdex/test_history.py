#!/usr/bin/env python3

from pytest_eosiocdt import (
    random_token_symbol,
    collect_stdout,
    name_to_string
)

from .constants import telosbookdex


def test_order_check_history(telosbookdex):

    buyer = telosbookdex.testnet.new_account()
    ec, buyer_id = telosbookdex.new_client(admin=buyer)

    supply = 1000
    symbol, precision, seller, seller_id = telosbookdex.init_test_token(
        max_supply=supply,
        precision=8
    )
    
    amount = 300
    price = 1000
    total = amount * price

    str_amount = format(amount, f'.{precision}f')
    str_asset_amount = f'{str_amount} {symbol}'
    str_asset_price = f'{price:.4f} TLOS'
    str_asset_total = f'{total:.4f} TLOS'

    # place sell order
    ec, out = telosbookdex.place_order(
        seller,
        'sell',
        str_asset_amount,
        str_asset_price,
        seller_id
    )

    assert ec == 0

    # generate buy order
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

    # place buy order
    ec, out = telosbookdex.place_order(
        buyer,
        'buy',
        str_asset_amount,
        str_asset_price,
        buyer_id
    )

    assert ec == 0

    market = telosbookdex.get_market(symbol.lower(), 'tlos')
    assert market is not None

    market_id = name_to_string(market['id'])
    
    history = telosbookdex.get_table(market_id, 'history')

    assert len(history) == 1

    trade = history[0]

    assert trade['buyer'] == buyer
    assert trade['seller'] == seller
    assert trade['price'] == str_asset_price
    assert trade['amount'] == str_asset_amount
    assert trade['payment'] == str_asset_total
