#!/usr/bin/env python3

"""Arithmetic Functions Testing

All this tests compare the results of operations using python's ``decimal``
module, with the results of the equivalent operation performed using
``vapaee::dex::utils`` arithmetic functions (``inverse`` & ``multiply``).

All test are parameterized, that means they will be called several times with
different parameters, in this case diferent precision settings for the eosio
assets to be operated on.

About ``decimal``: excerpt from ``docs.python.org/3/library/decimal.html``:

The decimal module provides support for fast correctly-rounded decimal floating
point arithmetic. It offers several advantages over the float datatype:

- Decimal “is based on a floating-point model which was designed with people in
mind, and necessarily has a paramount guiding principle – computers must
provide an arithmetic that works in the same way as the arithmetic that people
learn at school.” – excerpt from the decimal arithmetic specification.

- Decimal numbers can be represented exactly. In contrast, numbers like 1.1 and
2.2 do not have exact representations in binary floating point. End users
typically would not expect 1.1 + 2.2 to display as 3.3000000000000003 as it
does with binary floating point.

- The exactness carries over into arithmetic. In decimal floating point,
0.1 + 0.1 + 0.1 - 0.3 is exactly equal to zero. In binary floating point, the
result is 5.5511151231257827e-017. While near to zero, the differences prevent
reliable equality testing and differences can accumulate. For this reason,
decimal is preferred in accounting applications which have strict equality
invariants.

- The decimal module incorporates a notion of significant places so that
1.30 + 1.20 is 2.50. The trailing zero is kept to indicate significance. This
is the customary presentation for monetary applications. For multiplication,
the “schoolbook” approach uses all the figures in the multiplicands. For
instance, 1.3 * 1.2 gives 1.56 while 1.30 * 1.20 gives 1.5600.

- Unlike hardware based binary floating point, the decimal module has a user
alterable precision (defaulting to 28 places) which can be as large as needed
for a given problem.

- Both binary and decimal floating point are implemented in terms of published
standards. While the built-in float type exposes only a modest portion of its
capabilities, the decimal module exposes all required parts of the standard.
When needed, the programmer has full control over rounding and signal handling.
This includes an option to enforce exact arithmetic by using exceptions to
block any inexact operations.

- The decimal module was designed to support “without prejudice, both exact
unrounded decimal arithmetic (sometimes called fixed-point arithmetic) and
rounded floating-point arithmetic.” – excerpt from the decimal arithmetic
specification.

"""
import random

from decimal import Decimal, getcontext, ROUND_DOWN

import pytest

from pytest_eosiocdt import (
    collect_stdout,
    asset_from_decimal,
    asset_from_ints
)

# set ``Decimal`` precision to 32 digits for all tests
getcontext().prec = 32

@pytest.mark.parametrize(
    'a_prec,b_prec',
    [
        pytest.param(8, 8, id='equal'),
        pytest.param(8, 4, id='greater'),
        pytest.param(4, 8, id='lesser')
    ]
)
def test_inverse(eosio_testnet, a_prec, b_prec):
    """Create random decimal numbers, calculate their inverse using python
    then compare the results to the same operation using
    ``vapaee::dex::utils::inverse``
    """
    num_length = 20
    min_dec = -10000
    max_dec = 10000

    max_err = Decimal(10 ** -b_prec)

    numbers = [
        Decimal(
            '{num:.{prec}f}'.format(
                num=random.uniform(min_dec, max_dec), prec=a_prec
            )
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

        assert getcontext().abs(Decimal(pre) - Decimal(post)) <= max_err



@pytest.mark.parametrize(
    'a_prec,b_prec',
    [
        pytest.param(8, 8, id='equal'),
        pytest.param(8, 4, id='greater'),
        pytest.param(4, 8, id='lesser')
    ]
)
def test_multiply(eosio_testnet, a_prec, b_prec):
    """Create random decimal numbers, calculate their product using python then
    compare the results to the same operation using
    ``vapaee::dex::utils::multiply``
    """
    num_length = 20
    min_dec = -10000
    max_dec = 10000

    max_err = Decimal(1)

    a_numbers = [
        Decimal(
            '{num:.{prec}f}'.format(
                num=random.uniform(min_dec, max_dec), prec=a_prec
            )
        ) for _ in range(num_length)
    ]
    b_numbers = [
        Decimal(
            '{num:.{prec}f}'.format(
                num=random.uniform(min_dec, max_dec), prec=b_prec
            )
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

        assert getcontext().abs(Decimal(pre) - Decimal(post)) <= max_err


@pytest.mark.parametrize(
    'a_prec,b_prec',
    [
        pytest.param(8, 8, id='equal'),
        pytest.param(8, 4, id='greater'),
        pytest.param(4, 8, id='lesser')
    ]
)
def test_multiply_big_numbers(eosio_testnet, a_prec, b_prec):
    """Create big random integers, calculate their product using python then
    compare the results to the same operation using
    ``vapaee::dex::utils::multiply``
    """    
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
