#!/usr/bin/env python3

from .constants import telosbookdex


def test_add_token(telosbookdex):
    """Adds a new token to the exchange using the helper ``init_test_token``,  then checks if the 
    token exists and verifies the token parameters
    """
    symbol, precision, account, account_id = telosbookdex.init_test_token() 

    token = telosbookdex.get_token(symbol)

    assert token is not None
    assert token['admin'] == account
    assert token['precision'] == precision
