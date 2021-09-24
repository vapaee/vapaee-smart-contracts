#!/usr/bin/env python3

import random
import logging

from pytest_eosiocdt import Asset, Symbol

from .constants import telosbookdex


def test_market_id(telosbookdex):

    for i in range(6):
        logging.info(f"ITERATION {i}")
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
        # logging.info(market)
        # breakpoint()
        assert market['id'] % 2 == 0
