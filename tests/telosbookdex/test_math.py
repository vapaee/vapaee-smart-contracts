#!/usr/bin/env python3

import random

from decimal import Decimal, getcontext, ROUND_DOWN

import pytest

from pytest_eosiocdt import (
    collect_stdout,
    asset_from_decimal,
    asset_from_ints
)


@pytest.mark.parametrize(
    'a_prec,b_prec',
    [
        pytest.param(8, 8, id='equal'),
        pytest.param(8, 4, id='greater'),
        pytest.param(4, 8, id='lesser')
    ]
)
def test_inverse(eosio_testnet, a_prec, b_prec):

    getcontext().prec = 32
    num_length = 20
    min_dec = -10000
    max_dec = 10000

    numbers = [
        Decimal(
            '{num:.{prec}f}'.format(num=random.uniform(min_dec, max_dec), prec=a_prec)
        ) for _ in range(num_length)
    ]
    pre_results = [
        '{num:.{prec}f}'.format(
            num=Decimal(1) / a_num,
            prec=b_prec
        )
        for a_num in numbers
    ]

    assets = [
        asset_from_decimal(number, a_prec, 'TLOS') for number in numbers
    ]

    post_results = []
    for i, asset in enumerate(assets):
        ec, out = eosio_testnet.push_action(
            'testcontract',
            'inversetest',
            [asset, f'{b_prec},ACORN'],
            'eosio@active'
        )
        assert ec == 0
        
        post_results.append(collect_stdout(out).split(' ')[0])

    for pre, post in zip(pre_results, post_results):
        print(f'pre: {pre} post: {post}')

        # allow diference of 20 of the less significant units
        assert abs(float(pre) - float(post)) < 20



@pytest.mark.parametrize(
    'a_prec,b_prec',
    [
        pytest.param(8, 8, id='equal'),
        pytest.param(8, 4, id='greater'),
        pytest.param(4, 8, id='lesser')
    ]
)
def test_multiply(eosio_testnet, a_prec, b_prec):

    getcontext().prec = 32
    num_length = 20
    min_dec = -10000
    max_dec = 10000

    a_numbers = [
        Decimal(
            '{num:.{prec}f}'.format(num=random.uniform(min_dec, max_dec), prec=a_prec)
        ) for _ in range(num_length)
    ]
    b_numbers = [
        Decimal(
            '{num:.{prec}f}'.format(num=random.uniform(min_dec, max_dec), prec=b_prec)
        ) for _ in range(num_length)
    ]
    pre_results = [
        ('{num:.{prec}f}'.format(
            num=a_num * b_num,
            prec=b_prec
        ).replace('.', '')).lstrip('0')
        for a_num, b_num in zip(a_numbers, b_numbers)
    ]

    a_assets = [
        asset_from_decimal(a_number, a_prec, 'TLOS') for a_number in a_numbers
    ]
    b_assets = [
        asset_from_decimal(b_number, b_prec, 'TLOS') for b_number in b_numbers
    ]

    post_results = []
    for i, assets in enumerate(zip(a_assets, b_assets)):
        ec, out = eosio_testnet.push_action(
            'testcontract',
            'multiplytest',
            assets,
            'eosio@active'
        )
        assert ec == 0
        
        post_results.append(collect_stdout(out))

    for pre, post in zip(pre_results, post_results):
        print(f'pre: {pre} post: {post}')

        # allow diference of 20 of the less significant units
        assert abs(int(pre) - int(post)) < 20


@pytest.mark.parametrize(
    'a_prec,b_prec',
    [
        pytest.param(8, 8, id='equal'),
        pytest.param(8, 4, id='greater'),
        pytest.param(4, 8, id='lesser')
    ]
)
def test_multiply_big_numbers(eosio_testnet, a_prec, b_prec):
    num_length = 20

    # 9 billion 223 million 372 thousand 36
    min_num = -9223372036
    max_num = 9223372036

    a_numbers = [
        random.randint(min_num, max_num) for _ in range(num_length)
    ]
    b_numbers = [
        random.randint(min_num, max_num) for _ in range(num_length)
    ]
    pre_results = [
        a_number * b_number
        for a_number, b_number in zip(a_numbers, b_numbers)
    ]

    a_assets = [
        asset_from_ints(a_num * (10 ** a_prec), a_prec, 'TLOS') for a_num in a_numbers
    ]
    b_assets = [
        asset_from_ints(b_num * (10 ** b_prec), b_prec, 'TLOS') for b_num in b_numbers
    ]

    post_results = []
    for i, assets in enumerate(zip(a_assets, b_assets)):
        ec, out = eosio_testnet.push_action(
            'testcontract',
            'multiplytest',
            assets,
            'eosio@active'
        )
        assert ec == 0
        
        post_results.append(collect_stdout(out)[:-b_prec])

    i = 0
    for pre, post in zip(pre_results, post_results):
        print(f'a: {a_numbers[i]}  b: {b_numbers[i]}')
        print(f'pre: {pre} post: {post}')

        assert int(pre) == int(post)
        i += 1
