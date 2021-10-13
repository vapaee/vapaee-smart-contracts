#!/usr/bin/env python3

import random
import logging

from pytest_eosiocdt import Asset, Symbol

from .constants import telosbookdex


def test_market_id(telosbookdex):
    """The idea of this test is to create a lot of markets on book dex to
    display an intermitent bug were sometimes the id of the market is not
    divisible by two.

    I think the problem is in:
        ``vapaee::dex::market::aux_get_canonical_index_for_symbols``

    Called by:
        ``vapaee::dex::market::aux_create_market``

    Sometimes, tokens get "swapped" (commodity is currency and viceversa)
    by the function, and my current theory is that the execution reaches 
    the most indented block inside the if-else, which ends up ordering the
    tokens by their alphabetical order, and this of course can sometimes
    return the swapped index (50% chance pretty much).
    """

    for i in range(6):
        logging.info(f"ITERATION {i}")

        """Generate random tokens one will be the commodity and the other
        the currency
        """
        comm_scale = random.randrange(1, 100)
        curr_scale = random.randrange(1, 100)

        commodity_amount = comm_scale * 100000
        commodity_reserve_amount = comm_scale * 50000
        commodity_precision = 4
        
        currency_amount = curr_scale * 100000
        currency_reserve_amount = curr_scale * 50000
        currency_precision = 4

        token_data = telosbookdex.init_test_token(
            max_supply=commodity_amount + 1,
            precision=commodity_precision
        )

        sym, precision, commodity_seller, commodity_seller_id = token_data

        commodity_supply = Asset(commodity_amount, Symbol(sym, precision))

        token_data = telosbookdex.init_test_token(
            max_supply=currency_amount + 1,
            precision=currency_precision
        )

        sym, precision, currency_seller, currency_seller_id = token_data

        currency_supply = Asset(currency_amount, Symbol(sym, precision))

        # create token group
        ec, group_id = telosbookdex.new_token_group(currency_seller)
        assert ec == 0

        ec, _ = telosbookdex.change_token_groups(
            commodity_seller,
            commodity_supply.symbol.code,
            [group_id]
        )
        assert ec == 0
        ec, _ = telosbookdex.change_token_groups(
            currency_seller,
            currency_supply.symbol.code,
            [group_id]
        )
        assert ec == 0

        # set currency
        ec, _ = telosbookdex.set_currency(
            currency_seller,
            currency_supply.symbol.code,
            True,
            group_id
        )
        assert ec == 0

        telosbookdex.testnet.wait_blocks(10)

        # place dummy order to create markets
        dummy_comm = Asset(1, commodity_supply.symbol)
        dummy_curr = Asset(1, currency_supply.symbol) 
        ec, _ = telosbookdex.place_order(
            commodity_seller, 'sell', dummy_comm, dummy_curr, commodity_seller_id)
        assert ec == 0

        market = telosbookdex.get_market(
            commodity_supply.symbol.code, currency_supply.symbol.code)
        assert market
        assert market['id'] % 2 == 0
