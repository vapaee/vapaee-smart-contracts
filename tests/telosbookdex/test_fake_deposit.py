#!/usr/bin/env python3

from pytest_eosiocdt import random_token_symbol

from .constants import telosbookdex


def test_fake_deposit(telosbookdex):

    token_acc = telosbookdex.testnet.new_account()

    ec, token_acc_id = telosbookdex.new_client(admin=token_acc)
    
    supply = 1000
    precision = 8 
    symbol = random_token_symbol()
    str_amount = format(supply, f'.{precision}f')
    max_supply = f'{str_amount} {symbol}'

    telosbookdex.testnet.create_token(token_acc_id, max_supply)
    telosbookdex.testnet.issue_token(token_acc_id, max_supply, '')

    ec, out = telosbookdex.deposit(
        token_acc,
        max_supply
    )

    assert ec == 1
    assert "The token is not registered"
