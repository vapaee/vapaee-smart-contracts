#!/usr/bin/env python3

import json

from pytest_eosiocdt import random_token_symbol

from .constants import TelosBookDEX, telosbookdex


def test_order(telosbookdex):

    buyer = telosbookdex.testnet.new_account()
    seller = telosbookdex.testnet.new_account()

    eca, buyer_id = telosbookdex.new_client(admin=buyer)
    ecb, seller_id = telosbookdex.new_client(admin=seller)
    
    assert (eca + ecb) == 0

    supply = 1000
    precision = 8 
    symbol = random_token_symbol()
    str_amount = format(supply, f'.{precision}f')
    max_supply = f'{str_amount} {symbol}'

    telosbookdex.testnet.create_token(seller, max_supply)
    telosbookdex.testnet.issue_token(seller, max_supply, '')
    telosbookdex.add_token(seller, 'eosio.token', symbol, precision, seller)

    telosbookdex.deposit(
        seller,
        max_supply
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

    buy_table, sell_table = telosbookdex.get_order_book(symbol, 'TLOS')

    assert len(buy_table) == 1
    assert len(sell_table) == 0

    order = buy_table[0]

    assert order['client'] == seller_id
    assert order['owner'] == seller
    assert order['price'] == str_asset_price
    assert order['total'] == str_asset_total
    assert order['selling'] == str_asset_amount

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

    buy_table, sell_table = telosbookdex.get_order_book(symbol, 'TLOS')

    assert len(buy_table) == 0
    assert len(sell_table) == 0
