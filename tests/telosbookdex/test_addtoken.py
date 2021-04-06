#!/usr/bin/env python3

from pytest_eosiocdt import random_token_symbol

from .constants import TelosBookDEX, telosbookdex


def test_add_token(telosbookdex):
    account = telosbookdex.testnet.new_account()

    amount = 1000
    precision = 2
    symbol = random_token_symbol()
    str_amount = format(amount, f'.{precision}f')
    max_supply = f'{str_amount} {symbol}'
    
    telosbookdex.testnet.create_token(account, max_supply)
    telosbookdex.testnet.issue_token(account, max_supply, '')

    ec, out = telosbookdex.add_token(
        account,
        'eosio.token',
        symbol,
        precision,
        account
    )

    assert ec == 0

    token = telosbookdex.get_token(symbol)

    assert token is not None
    assert token['admin'] == account
    assert token['precision'] == precision
