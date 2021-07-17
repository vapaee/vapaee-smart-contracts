#!/usr/bin/env python3

from pytest_eosiocdt import random_token_symbol


def test_initpool(eosio_testnet):

    poolname = 'koinonospool'

    admin_accounts = [
        eosio_testnet.new_account(), 
        eosio_testnet.new_account()
    ]
    
    # create tokens
    symbols = [
        random_token_symbol(),
        random_token_symbol()
    ]

    # init pools
    for admin, symbol in zip(admin_accounts, symbols):

        proposal = eosio_testnet.multi_sig_propose(
            admin,
            [f'{poolname}@active', f'{admin}@active'],
            [f'{poolname}@active', f'{admin}@active'],
            poolname,
            'initpool',
            {
                'admin': admin,
                'token_contract': 'eosio.token',
                'token_sym': f'2,{symbol}'
            }
        )

        for approver in [poolname, admin]:
            ec, _ = eosio_testnet.multi_sig_approve(
                admin,
                proposal,
                [f'{approver}@active'],
                approver
            )
            assert ec == 0

        ec, _ = eosio_testnet.multi_sig_exec(
            admin, proposal, admin)

        search = [
            pool 
            for pool in eosio_testnet.get_table(
                poolname,
                poolname,
                'pools'
            )
            if pool['admin'] == admin
        ]

        assert len(search) == 1
        
        pool = search[0]
        assert pool['token_contract'] == 'eosio.token'
        assert pool['reserve'] == f'0.00 {symbol}'

    supplys = [
        f'900000.00 {sym}' for sym in symbols
    ]

    pool_supp_amounts = [10000, 20000]

    pool_supplys = [
        f'{amount:.2f} {sym}'
        for amount, sym in zip(pool_supp_amounts, symbols)
    ]

    for creator, symbol, max_supply, pool_supply in zip(
        admin_accounts,
        symbols,
        supplys,
        pool_supplys
    ):
        ec, _ = eosio_testnet.create_token(creator, max_supply)
        assert ec == 0
        ec, _ = eosio_testnet.issue_token(creator, pool_supply, '')
        assert ec == 0
    
        # fund pool
        ec, out = eosio_testnet.transfer_token(
            creator,
            poolname,
            pool_supply,
            'fund'
        )
        assert ec == 0
        
        search = [
            pool 
            for pool in eosio_testnet.get_table(
                poolname,
                poolname,
                'pools'
            )
            if pool['admin'] == creator
        ]

        assert len(search) == 1
        
        pool = search[0]
        assert pool['token_contract'] == 'eosio.token'
        assert pool['reserve'] == pool_supply 


    sender = eosio_testnet.new_account()
    recipient = eosio_testnet.new_account()
    
    admin_a, admin_b = admin_accounts
    sym_a, sym_b = symbols
    
    convert_amount = 100
    convert_asset = f'{convert_amount:.2f} {sym_a}'
    total_amount = 50
    min_asset = f'{total_amount:.2f} {sym_b}'

    ec, _ = eosio_testnet.issue_token(admin_a, convert_asset, '')
    assert ec == 0
    ec, _ = eosio_testnet.transfer_token(admin_a, sender, convert_asset, '')
    assert ec == 0

    # convert 100 tokens through to recipient
    ec, _ = eosio_testnet.transfer_token(
        sender,
        poolname,
        convert_asset,
        f'koinonos.v1,{min_asset},{recipient}'
    )

    assert ec == 0
   
    pools = eosio_testnet.get_table(
        poolname,
        poolname,
        'pools'
    )

    assert len(pools) == 2

    pool_a, pool_b = pools
    supply_a, supply_b = pool_supp_amounts

    assert pool_a['reserve'] == f'{supply_a + convert_amount:.2f} {sym_a}'
    assert pool_b['reserve'] == f'{supply_b - total_amount:.2f} {sym_b}'
