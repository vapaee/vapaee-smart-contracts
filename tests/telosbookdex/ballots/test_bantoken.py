#!/usr/bin/env python3

import time
import random

from pytest_eosiocdt import random_token_symbol
from pytest_eosiocdt.telos import telosdecide
from pytest_eosiocdt.sugar import name_to_string

from ..constants import telosbookdex


def test_ballot_on_bantoken_yes(telosdecide, telosbookdex):
    vote_symbol, vote_precision, vote_supply = telosbookdex.init_dao(telosdecide)
    token_acc = telosbookdex.testnet.new_account()
    _, token_acc_id = telosbookdex.new_client(admin=token_acc)
    ballot_acc = telosbookdex.testnet.new_account()
    telosdecide.register_voter(
        ballot_acc, f'{vote_precision},{vote_symbol}', ballot_acc
    )
    telosbookdex.testnet.give_token(
        ballot_acc,
        '100.0000 TLOS'
    )
    telosbookdex.deposit(ballot_acc, '10.0000 TLOS')

    max_supply = 10000
    precision = 2
    symbol = random_token_symbol()
    str_max_supply = format(max_supply, f'.{precision}f')
    str_max_supply = f'{str_max_supply} {symbol}'

    price = 10000
    amount = 1000
    total = price * amount
    str_amount = format(amount, f'.{precision}f')
    str_asset_amount = f'{str_amount} {symbol}'
    str_asset_price = f'{price:.4f} TLOS'
    str_asset_total = f'{total:.4f} TLOS'
    
    telosbookdex.testnet.create_token(token_acc, str_max_supply)
    telosbookdex.testnet.issue_token(token_acc, str_max_supply, '')
    telosbookdex.add_token(
        token_acc,
        'eosio.token',
        symbol,
        precision,
        token_acc
    )
    telosbookdex.deposit(
        token_acc,
        str_max_supply
    )

    # place sell order
    ec, out = telosbookdex.place_order(
        token_acc,
        'sell',
        str_asset_amount,
        str_asset_price,
        token_acc_id
    )

    assert ec == 0

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

    total_voters = 10
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

    start_time = time.time()

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

    options = [*[['yes'] for _ in range(8)], ['no']]
    ballot_name = name_to_string(ballot_info['id'])
    current_time = 0
    for voter in voters:
        current_time = time.time()
        if (current_time - start_time) > 3:
            break
        ec, _ = telosdecide.cast_vote(
            voter, ballot_name, random.choice(options)
        )

    remaining = 5 - (current_time - start_time)
    if remaining > 0:
        time.sleep(remaining + 1)

    ec, _ = telosdecide.close_voting(ballot_name)
    assert ec == 0

    ballot_info = next(
        (row
        for row in telosbookdex.get_ballots()
        if row['id'] == ballot_info['id']),
        None
    )

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    total_vote = 0
    for option in ballot_info['results']:
        total_vote += float(option['value'].split(' ')[0])

    assert total_vote == total_voters * voting_power

    # place sell order
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
    vote_symbol, vote_precision, vote_supply = telosbookdex.init_dao(telosdecide)
    token_acc = telosbookdex.testnet.new_account()
    _, token_acc_id = telosbookdex.new_client(admin=token_acc)
    ballot_acc = telosbookdex.testnet.new_account()
    telosdecide.register_voter(
        ballot_acc, f'{vote_precision},{vote_symbol}', ballot_acc
    )
    telosbookdex.testnet.give_token(
        ballot_acc,
        '100.0000 TLOS'
    )
    telosbookdex.deposit(ballot_acc, '10.0000 TLOS')

    max_supply = 10000
    precision = 2
    symbol = random_token_symbol()
    str_max_supply = format(max_supply, f'.{precision}f')
    str_max_supply = f'{str_max_supply} {symbol}'

    price = 10000
    amount = 1000
    total = price * amount
    str_amount = format(amount, f'.{precision}f')
    str_asset_amount = f'{str_amount} {symbol}'
    str_asset_price = f'{price:.4f} TLOS'
    str_asset_total = f'{total:.4f} TLOS'
    
    telosbookdex.testnet.create_token(token_acc, str_max_supply)
    telosbookdex.testnet.issue_token(token_acc, str_max_supply, '')
    telosbookdex.add_token(
        token_acc,
        'eosio.token',
        symbol,
        precision,
        token_acc
    )
    telosbookdex.deposit(
        token_acc,
        str_max_supply
    )

    # place sell order
    ec, out = telosbookdex.place_order(
        token_acc,
        'sell',
        str_asset_amount,
        str_asset_price,
        token_acc_id
    )

    assert ec == 0

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

    total_voters = 10
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

    start_time = time.time()

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

    options = [*[['no'] for _ in range(8)], ['yes']]
    ballot_name = name_to_string(ballot_info['id'])
    current_time = 0
    for voter in voters:
        current_time = time.time()
        if (current_time - start_time) > 3:
            break
        ec, _ = telosdecide.cast_vote(
            voter, ballot_name, random.choice(options)
        )

    remaining = 5 - (current_time - start_time)
    if remaining > 0:
        time.sleep(remaining + 1)

    ec, _ = telosdecide.close_voting(ballot_name)
    assert ec == 0

    ballot_info = next(
        (row
        for row in telosbookdex.get_ballots()
        if row['id'] == ballot_info['id']),
        None
    )

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    total_vote = 0
    for option in ballot_info['results']:
        total_vote += float(option['value'].split(' ')[0])

    assert total_vote == total_voters * voting_power

    # place sell order
    ec, out = telosbookdex.place_order(
        token_acc,
        'sell',
        str_asset_amount,
        str_asset_price,
        token_acc_id
    )

    assert ec == 0
