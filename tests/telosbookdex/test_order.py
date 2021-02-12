#!/usr/bin/env python3

from pytest_eosiocdt import random_token_symbol

from .constants import TelosBookDEX, telosbookdex


def test_order(telosbookdex):

    buyer = telosbookdex.testnet.new_account()
    telosbookdex.testnet.transfer_token(
        'eosio.token',
        buyer,
        f'{1000:.8f} TLOS',
        ''
    )
    
    seller = telosbookdex.testnet.new_account()

    eca, buyer_id = telosbookdex.new_client(admin=buyer)
    ecb, seller_id = telosbookdex.new_client(admin=seller)
    
    assert (eca + ecb) == 0

    amount = 1000
    precision = 8
    symbol = random_token_symbol()
    str_amount = format(amount, f'.{precision}f')
    max_supply = f'{str_amount} {symbol}'

    telosbookdex.testnet.create_token(seller, max_supply)
    telosbookdex.testnet.issue_token(seller, max_supply, '')
    telosbookdex.add_token(seller, 'eosio.token', symbol, precision, seller)

    telosbookdex.deposit(
        seller,
        max_supply
    )
    
    sell_amount = 300
    sell_price = 1000

    str_sell_amount = format(sell_amount, f'.{precision}f')
    sell_asset_total = f'{str_sell_amount} {symbol}'
    sell_asset_price = f'{sell_price:.8f} TLOS'

    # place sell order
    ec, out = telosbookdex.place_order(
        seller,
        'sell',
        sell_asset_total,
        sell_asset_price,
        seller_id
    )

    assert ec == 0
