#!/usr/bin/env python3

from pytest_eosiocdt import random_token_symbol

from .constants import telosbookdex


def test_order(telosbookdex):
    """Perform order execution and check that the order book gets updated
    correctly. Also check user reward tables
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

    # check order is placed in book
    buy_table, sell_table = telosbookdex.get_order_book(symbol, 'TLOS')

    assert len(buy_table) == 1
    assert len(sell_table) == 0

    order = buy_table[0]

    assert order['client'] == seller_id
    assert order['owner'] == seller
    assert order['price'] == str_asset_price
    assert order['total'] == str_asset_total
    assert order['selling'] == str_asset_amount

    # place buy order (should consume sell order)
    ec, out = telosbookdex.place_order(
        buyer,
        'buy',
        str_asset_amount,
        str_asset_price,
        buyer_id
    )

    assert ec == 0

    # check order book is empty
    buy_table, sell_table = telosbookdex.get_order_book(symbol, 'TLOS')

    assert len(buy_table) == 0
    assert len(sell_table) == 0

    # check rewards
    config = telosbookdex.get_config()

    maker_exp = telosbookdex.get_client_experience(seller)
    maker_pts = telosbookdex.get_client_points(seller)

    taker_exp = telosbookdex.get_client_experience(buyer)
    taker_pts = telosbookdex.get_client_points(buyer)

    assert maker_exp == total * float(config['maker_exp_reward'])
    assert maker_pts == total * float(config['maker_pts_reward'])

    assert taker_exp == total * float(config['taker_exp_reward'])
    assert taker_pts == total * float(config['taker_pts_reward'])

