#!/usr/bin/env python3

import logging

from pytest_eosiocdt import (
    eosio_testnet,
    random_eosio_name,
    random_token_symbol,
    collect_stdout,
    string_to_sym_code,
    Name
)

from .telosbookdex.constants import get_market_index


def test_sym_code_parsing(eosio_testnet):
    sym = random_token_symbol()

    ec, out = eosio_testnet.push_action(
        'testcontract',
        'rawsymcode',
        [sym],
        'eosio@active'
    )
    assert ec == 0

    correct = int(collect_stdout(out))
    actual = string_to_sym_code(sym)
    logging.info(
        f'\ncorrect: {correct}'
        f'\nactual:  {actual}')

    assert correct == actual 


def test_name_parsing(eosio_testnet):
    name = random_eosio_name()

    ec, out = eosio_testnet.push_action(
        'testcontract',
        'rawname',
        [name],
        'eosio@active'
    )
    assert ec == 0

    correct = int(collect_stdout(out))
    actual = Name(name).value
    logging.info(
        f'\ncorrect: {correct}'
        f'\nactual:  {actual}')

    assert correct == actual 


def test_symbols_get_index(eosio_testnet):
    a, b = (random_token_symbol(), random_token_symbol())

    ec, out = eosio_testnet.push_action(
        'testcontract',
        'getindex',
        [a, b],
        'eosio@active'
    )
    assert ec == 0

    correct = int(collect_stdout(out))
    actual = get_market_index(a, b)
    logging.info(
        f'\ncorrect: {correct}'
        f'\nactual:  {actual}')

    assert  correct == actual 

