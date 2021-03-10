#!/usr/bin/env python3

import time
import random

from pytest_eosiocdt import random_token_symbol
from pytest_eosiocdt.telos import telosdecide
from pytest_eosiocdt.sugar import name_to_string

from ..constants import telosbookdex


def test_ballot_on_bantoken(telosdecide, telosbookdex):
    vote_symbol, vote_precision, vote_supply = telosbookdex.init_dao(telosdecide)
    token_acc = telosbookdex.testnet.new_account()
    ballot_acc = telosbookdex.testnet.new_account()
    telosdecide.register_voter(
        ballot_acc, f'{vote_precision},{vote_symbol}', ballot_acc
    )
    telosbookdex.testnet.give_token(
        ballot_acc,
        '100.0000 TLOS'
    )
    telosbookdex.deposit(ballot_acc, '10.0000 TLOS')

    amount = 1000
    precision = 2
    symbol = random_token_symbol()
    str_amount = format(amount, f'.{precision}f')
    max_supply = f'{str_amount} {symbol}'
    
    telosbookdex.testnet.create_token(token_acc, max_supply)
    telosbookdex.testnet.issue_token(token_acc, max_supply, '')
    telosbookdex.add_token(
        token_acc,
        'eosio.token',
        symbol,
        precision,
        token_acc
    )

    voting_power = 1000
    voting_amount = format(voting_power, f'.{vote_precision}f')
    def init_voter():
        voter = telosbookdex.testnet.new_account()
        telosdecide.register_voter(
            voter, f'{vote_precision},{vote_symbol}', voter
        )
        telosdecide.mint(
            voter,
            f'{voting_amount} {vote_symbol}',
            f'bantoken {symbol} eosio.token'
        )
        return voter

    total_voters = 30
    voters = [
        init_voter()
        for _ in range(total_voters)
    ]

    ec, out = telosbookdex.dao_bantoken(
        symbol,
        'eosio.token',
        ballot_acc
    )

    assert ec == 0

    ballot_info = next(
        (row
        for row in telosbookdex.get_ballots()
        if row['operation'] == 'bantoken' and
        len(row['params']) == 2 and
        row['params'][0] == symbol and
        row['params'][1] == 'eosio.token' and
        row['feepayer'] == ballot_acc),
        None
    )
    assert ballot_info is not None

    options = [['yes'], ['no'], ['abstain']]
    ballot_name = name_to_string(ballot_info['id'])
    for voter in voters:
        ec, _ = telosdecide.cast_vote(
            voter, ballot_name, random.choice(options)
        )
        assert ec == 0

    time.sleep(5)  # ballot finishes

    ec, _ = telosdecide.close_voting(ballot_name)
    assert ec == 0

    ballot_info = next(
        (row
        for row in telosbookdex.get_ballots()
        if row['id'] == ballot_info['id']),
        None
    )

    total_vote = 0
    for option in ballot_info['results']:
        total_vote += float(option['value'].split(' ')[0])

    assert total_vote == total_voters * voting_power
