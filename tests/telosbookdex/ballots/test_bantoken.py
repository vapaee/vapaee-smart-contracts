#!/usr/bin/env python3

from pytest_eosio.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_bantoken_yes(telosdecide, telosbookdex):
    """Generate a new token & seller account, and place a sell order, then
    approve a vote to ban that token, attempt to place a sell order same as
    before, this time it should fail, as the token was banned
    """
    # generate token & seller account
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    # generate eosio asset strings
    price = 10000
    amount = 1000
    total = price * amount
    str_amount = format(amount, f'.{prec}f')
    str_asset_amount = f'{str_amount} {sym}'
    str_asset_price = f'{price:.4f} TLOS'
    str_asset_total = f'{total:.4f} TLOS'

    # place sell order
    ec, out = telosbookdex.place_order(
        token_acc,
        'sell',
        str_asset_amount,
        str_asset_price,
        token_acc_id
    )

    assert ec == 0

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_bantoken(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc)

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    # place sell order again
    ec, out = telosbookdex.place_order(
        token_acc,
        'sell',
        str_asset_amount,
        str_asset_price,
        token_acc_id
    )

    assert ec == 1
    assert 'Token is not registered' in out


def test_ballot_on_bantoken_no(telosdecide, telosbookdex):
    """Generate a new token & seller account, and place a sell order, then
    attemp a vote to ban that token (but fail), place a sell order same as
    before.
    """
    # generate token & seller account
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    # generate eosio asset strings
    price = 10000
    amount = 1000
    total = price * amount
    str_amount = format(amount, f'.{prec}f')
    str_asset_amount = f'{str_amount} {sym}'
    str_asset_price = f'{price:.4f} TLOS'
    str_asset_total = f'{total:.4f} TLOS'

    # place sell order
    ec, out = telosbookdex.place_order(
        token_acc,
        'sell',
        str_asset_amount,
        str_asset_price,
        token_acc_id
    )

    assert ec == 0

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_bantoken(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc)

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    # place sell order again
    ec, out = telosbookdex.place_order(
        token_acc,
        'sell',
        str_asset_amount,
        str_asset_price,
        token_acc_id
    )

    assert ec == 0
