#!/usr/bin/env python3

import random

import pytest

from pytest_eosiocdt import (
    random_token_symbol,
    asset_from_str,
    Symbol,
    Asset
)

from .constants import koinonospool


@pytest.mark.parametrize('i', range(10))
def test_initpool(koinonospool, i):
    """Creates two admin accounts, symbols & registers them as pools in
    koinonos, then funds both finally send some tokens to make
    conversion, verifies reserves get correctly updated.
    """

    admin_accounts = [
        koinonospool.testnet.new_account(), 
        koinonospool.testnet.new_account()
    ]
    
    symbols = [
        Symbol(random_token_symbol(), random.randint(2, 8)),
        Symbol(random_token_symbol(), random.randint(2, 8))
    ]

    # init pools
    for admin, symbol in zip(admin_accounts, symbols):
        ec, _ = koinonospool.init_pool(
            admin, 'eosio.token', symbol)
        assert ec == 0

        search = koinonospool.get_pools_by_admin(admin) 
        assert len(search) == 1
        
        pool = search[0]
        assert pool['token_contract'] == 'eosio.token'
        assert pool['reserve'] == str(Asset(0, symbol))

    # create tokens & fund pools

    max_amount = 900000

    supplys = [
        Asset(max_amount, sym)
        for sym in symbols
    ]

    pool_supplys = [
        Asset(random.randint(100, 99999), sym)
        for sym in symbols
    ]

    for creator, symbol, max_supply, pool_supply in zip(
        admin_accounts,
        symbols,
        supplys,
        pool_supplys
    ):
        ec, _ = koinonospool.testnet.create_token(creator, max_supply)
        assert ec == 0
        ec, _ = koinonospool.testnet.issue_token(creator, pool_supply, '')
        assert ec == 0
    
        ec, _ = koinonospool.fund_pool(creator, pool_supply)
        assert ec == 0
        
        search = koinonospool.get_pools_by_admin(creator) 
        assert len(search) == 1
        
        pool = search[0]
        assert pool['token_contract'] == 'eosio.token'
        assert pool['reserve'] == str(pool_supply)

    # create conversion participants
    sender = koinonospool.testnet.new_account()
    recipient = koinonospool.testnet.new_account()
    
    admin_a, admin_b = admin_accounts
    sym_a, sym_b = symbols
    pool_a_supp, pool_b_supp = pool_supplys
    
    # amount to convert
    convert_asset = Asset(random.randint(10, 100), sym_a)

    # figure out convertion on our own
    rate = pool_b_supp.amount / (pool_a_supp.amount + convert_asset.amount)
    total_amount = rate * convert_asset.amount

    min_asset = Asset(
        total_amount - 0.1,
        sym_b
    )

    # issue & transfer exact amount needed to sender
    ec, _ = koinonospool.testnet.issue_token(admin_a, convert_asset, '')
    assert ec == 0
    ec, _ = koinonospool.testnet.transfer_token(admin_a, sender, convert_asset, '')
    assert ec == 0

    # finally convert
    ec, _ = koinonospool.convert(
        sender,
        convert_asset,
        min_asset,
        recipient
    )
    assert ec == 0
   
    # verify pool reserves correctly updated
    pool_a = koinonospool.get_pools_by_admin(admin_a)[0]
    pool_b = koinonospool.get_pools_by_admin(admin_b)[0]

    supply_a, supply_b = pool_supplys

    assert asset_from_str(pool_a['reserve']).amount - Asset(
            supply_a.amount + convert_asset.amount, sym_a).amount <= 0.1

    assert asset_from_str(pool_b['reserve']).amount - Asset(
            supply_b.amount - total_amount, sym_b).amount <= 0.1
