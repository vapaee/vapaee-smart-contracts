#!/usr/bin/env python3

import time
import random

from typing import Optional, List
from contextlib import contextmanager

import pytest

from pytest_eosiocdt import (
    collect_stdout,
    name_to_string,
    random_string,
    random_local_url,
    random_token_symbol
)
from pytest_eosiocdt.telos import init_telos_token
from pytest_eosiocdt.contract import SmartContract


def get_market_scope(
    sym_a: str,
    sym_b: str
):
    return f'{sym_a.lower()}.{sym_b.lower()}'[:12]


_did_init = False
_did_dao_init = False
class TelosBookDEX(SmartContract):
    
    @property
    def contract_name(self) -> str:
        return 'telosbookdex'

    @property
    def dao_symbol(self):
        return ('VOTE', 4, 10000000000)

    def __init__(self, eosio_testnet):
        super().__init__(eosio_testnet)
        init_telos_token(eosio_testnet)

        global _did_init
        if not _did_init:
            ec, _ = self.push_action(
                'init',
                [],
                f'{self.contract_name}@active'
            )
            assert ec == 0 
            _did_init = True

    def get_config(self):
        return self.get_table(self.contract_name, 'state')[0];

    def new_client(
        self,
        admin: Optional[str] = None,
        receiver: Optional[str] = None
    ):
        if admin is None:
            admin = self.testnet.new_account()

        if receiver is None:
            receiver = self.testnet.new_account()

        params = '{}'
        title = random_string()
        website = random_local_url()
        brief = random_string()
        banner = random_local_url()
        thumbnail = random_local_url()

        ec, out = self.push_action(
            'addclient',
            [
                admin,
                receiver,
                params,
                title,
                website,
                brief,
                banner,
                thumbnail
            ],
            f'{admin}@active'
        )

        if ec == 0:
            # get client id
            clients = self.get_table(
                self.contract_name,
                'clients'
            )

            client = next((
                row for row in clients
                if row['admin'] == admin and
                   row['receiver'] == receiver and
                   row['params'] == params and
                   row['title'] == title and
                   row['website'] == website and
                   row['brief'] == brief and
                   row['banner'] == banner and
                   row['thumbnail'] == thumbnail),
                None
            )
            assert client is not None
            return ec, int(client['id'])
        else:
            return ec, out

    def get_token_groups(self):
        return self.get_table(
            self.contract_name,
            'tokengroups'
        )

    def new_token_group(
        self,
        admin: str
    ):
        title = random_string()
        website = random_local_url()
        brief = random_string()
        banner = random_local_url()
        thumbnail = random_local_url()

        ec, out = self.push_action(
            'addtnkgroup',
            [admin, title, website, brief, banner, thumbnail],
            f'{admin}@active'
        )

        if ec == 0:
            # get group id
            groups = self.testnet.get_table(
                self.contract_name,
                self.contract_name,
                'tokengroups'
            )

            group = next((
                row for row in groups
                if row['admin'] == admin and
                   row['title'] == title and
                   row['website'] == website and
                   row['brief'] == brief and
                   row['banner'] == banner and
                   row['thumbnail'] == thumbnail),
                None
            )
            assert group is not None
            return ec, int(group['id'])
        else:
            return ec, out

    def add_token(
        self,
        issuer: str,
        contract: str,
        symbol: str,
        precision: int,
        admin: str,
        tradeable: bool = True
    ):
        title = random_string()
        website = random_local_url()
        brief = random_string()
        banner = random_local_url()
        icon = random_local_url()
        iconlg = random_local_url()
        pcontract = random_string(size=12)
        gcontract = random_string(size=12)

        return self.push_action(
            'addtoken',
            [
                contract,
                symbol,
                precision,
                admin,
                title,
                website,
                brief,
                banner,
                icon,
                iconlg,
                pcontract,
                gcontract,
                tradeable
            ],
            f'{issuer}@active'
        )

    def change_token_groups(self, admin: str, sym: str, groups: List[int]):
        return self.push_action(
            'chnggroups',
            [sym, groups],
            f'{admin}@active'
        )

    def set_currency(self, sym: str, value: bool, group: int):
        return self.push_action(
            'setcurrency',
            [sym, value, group],
            f'{self.contract_name}@active'
        )

    def get_token(self, symbol: str):
        tokens = self.get_table(
            self.contract_name,
            'tokens'
        )
        
        return next((
            row for row in tokens
            if row['symbol'] == symbol),
            None
        )

    def get_blacklist(self):
        return self.get_table(
            self.contract_name,
            'blacklist'
        )

    def get_whitelist(self):
        return self.get_table(
            self.contract_name,
            'whitelist'
        )

    def deposit(
        self,
        account,
        quantity
    ):
        return self.testnet.transfer_token(
            account,
            self.contract_name,
            quantity,
            'deposit'
        )

    def get_market(
        self,
        sym_a: str,
        sym_b: str
    ):
        market_scope = get_market_scope(sym_a, sym_b)
        markets = self.get_table(
            self.contract_name,
            'markets',
            '--index', '2',  # 'table'
            '--lower', market_scope,
            '--key-type', 'name'
        )

        for market in markets:
            if market['table'] == market_scope:
                if (market['commodity'] == sym_a) and (market['currency'] == sym_b):
                    return market
            else:
                return None

    def get_order_book(
        self,
        symbol_a: str,
        symbol_b: str
    ):
        buy_market = self.get_market(symbol_a, symbol_b)
        sell_market = self.get_market(symbol_b, symbol_a)

        if (buy_market is None) or (sell_market is None):
            raise ValueError('market not found')

        buy_table = self.get_table(
            str(buy_market['id']),
            'sellorders'
        )

        sell_table = self.get_table(
            str(sell_market['id']),
            'sellorders'
        )

        return (buy_table, sell_table)

    def place_order(
        self,
        owner: str,
        _type: str,
        amount: str,
        price: str,
        client_id: int
    ):
        return self.push_action(
            'order',
            [
                owner,
                _type,
                amount,
                price,
                client_id
            ],
            f'{owner}@active'
        )

    def init_test_token(
        self,
        max_supply = 10000,
        precision = 2,
        symbol: Optional[str] = None
    ):
        if symbol is None:
            symbol = random_token_symbol()

        token_acc = self.testnet.new_account()
        _, token_acc_id = self.new_client(admin=token_acc)

        str_max_supply = format(max_supply, f'.{precision}f')
        str_max_supply = f'{str_max_supply} {symbol}'

        ec, _ = self.testnet.create_token(token_acc, str_max_supply)
        assert ec == 0
        ec, _ = self.testnet.issue_token(token_acc, str_max_supply, '')
        assert ec == 0
        ec, _ = self.add_token(
            token_acc,
            'eosio.token',
            symbol,
            precision,
            token_acc
        )
        assert ec == 0
        ec, _ = self.deposit(
            token_acc,
            str_max_supply
        )
        assert ec == 0

        return (symbol, precision, token_acc, token_acc_id)


    """DAO
    """

    def get_ballots(self):
        return self.get_table(
            self.contract_name,
            'ballots'
        )

    def get_ballot_by_feepayer(self, feepayer: str):
        return next(
            (row
            for row in self.get_ballots()
            if row['feepayer'] == feepayer),
            None
        )


    def init_dao(
        self,
        telosdecide,
        manager='eosio'
    ):
        global _did_dao_init
        if not _did_dao_init:
            vsym, vprec, vsupply = self.dao_symbol
            ec, _ = telosdecide.init('2.0.0')
            assert ec == 0
            quantity = '1000.0000 TLOS'
            ec, _ = self.testnet.give_token(manager, quantity)
            assert ec == 0
            ec, _ = telosdecide.deposit(manager, quantity)
            assert ec == 0
            vsupply = format(vsupply, f'.{vprec}f')
            vote_supply = f'{vsupply} {vsym}'
            ec, _ = telosdecide.new_treasury(manager, vote_supply, 'public')
            assert ec == 0
            ec, _ = telosdecide.register_voter(
                self.contract_name,
                f'{vprec},{vsym}',
                self.contract_name
            )
            assert ec == 0
            ec, _ = telosdecide.register_voter(
                'eosio',
                f'{vprec},{vsym}',
                'eosio'
            )
            assert ec == 0
            ec, _ == telosdecide.toggle(f'{vprec},{vsym}', 'reclaimable')
            assert ec == 0
            ec, _ == telosdecide.toggle(f'{vprec},{vsym}', 'burnable')
            assert ec == 0
            _did_dao_init = True

        return self.dao_symbol

    def ballot_on(
        self,
        operation: str,
        params: List[str],
        args: str,
        feepayer: str
    ):
        return self.push_action(
            'balloton',
            [
                operation,
                params,
                args,
                feepayer
            ],
            f'{feepayer}@active'
        )

    def dao_bantoken(
        self,
        symbol_code: str,
        contract: str,
        feepayer: str
    ):
        return self.ballot_on(
            'bantoken',
            [symbol_code, contract],
            f'bantoken {symbol_code} {contract}',
            feepayer
        )

    def dao_savetoken(
        self,
        symbol_code: str,
        contract: str,
        feepayer: str
    ):
        return self.ballot_on(
            'savetoken',
            [symbol_code, contract],
            f'savetoken {symbol_code} {contract}',
            feepayer
        )

    def dao_makerfee(
        self,
        amount: str,
        feepayer: str
    ):
        return self.ballot_on(
            'makerfee',
            [amount],
            f'makerfee {amount}',
            feepayer
        )

    def dao_takerfee(
        self,
        amount: str,
        feepayer: str
    ):
        return self.ballot_on(
            'takerfee',
            [amount],
            f'takerfee {amount}',
            feepayer
        )
     
    def dao_setcurrency(
        self,
        symbol_code: str,
        contract: str,
        feepayer: str
    ):
        return self.ballot_on(
            'setcurrency',
            [symbol_code, contract],
            f'setcurrency {symbol_code} {contract}',
            feepayer
        )

    def dao_regcost(
        self,
        cost: set,
        feepayer: str
    ):
        return self.ballot_on(
            'regcost',
            [cost],
            f'regcost {cost}',
            feepayer
        )

    def dao_approvalmin(
        self,
        _min: float,
        feepayer: str
    ):
        return self.ballot_on(
            'approvalmin',
            [_min],
            f'approvalmin {_min}',
            feepayer
        )

    def dao_historyprune(
        self,
        days: int,
        feepayer: str
    ):
        return self.ballot_on(
            'historyprune',
            [days],
            f'historyprune {days}',
            feepayer
        )

    def dao_hblockprune(
        self,
        days: int,
        feepayer: str
    ):
        return self.ballot_on(
            'hblockprune',
            [days],
            f'hblockprune {days}',
            feepayer
        )

    def dao_eventsprune(
        self,
        days: int,
        feepayer: str
    ):
        return self.ballot_on(
            'eventsprune',
            [days],
            f'eventsprune {days}',
            feepayer
        )

    def dao_pointsprune(
        self,
        weeks: int,
        feepayer: str
    ):
        return self.ballot_on(
            'pointsprune',
            [weeks],
            f'pointsprune {weeks}',
            feepayer
        )

    def dao_ballotsprune(
        self,
        max_entries: int,
        feepayer: str
    ):
        return self.ballot_on(
            'ballotsprune',
            [max_entries],
            f'ballotsprune {max_entries}',
            feepayer
        )


    @contextmanager
    def perform_vote(
        self,
        telosdecide,
        choices,
        total_voters = 10,
        voting_power = 1000,
    ):
        vote_symbol, vote_precision, vote_supply = self.init_dao(
            telosdecide
        )

        vote_sym_str = f'{vote_precision},{vote_symbol}'

        ballot_acc = self.testnet.new_account()

        ec, _ = telosdecide.register_voter(
            ballot_acc, vote_sym_str, ballot_acc 
        )
        assert ec == 0

        ec, _ = self.testnet.give_token(
            ballot_acc,
            '10.0000 TLOS'
        )
        assert ec == 0

        ec, _ = self.deposit(ballot_acc, '10.0000 TLOS')
        assert ec == 0    

        voting_amount = format(voting_power, f'.{vote_precision}f')
        def init_voter():
            voter = self.testnet.new_account()
            ec, _ = telosdecide.register_voter(
                voter, f'{vote_precision},{vote_symbol}', voter
            )
            assert ec == 0
            ec, _ = telosdecide.mint(
                voter,
                f'{voting_amount} {vote_symbol}',
                'perform vote'
            )
            assert ec == 0
            return voter

        voters = [  # voters
            init_voter()
            for _ in range(total_voters)
        ]

        yield (voters, ballot_acc)  # ballot account

        start_time = time.time()

        ballot_info = self.get_ballot_by_feepayer(ballot_acc) 
        assert ballot_info is not None

        ballot_name = name_to_string(ballot_info['id'])
        current_time = 0
        for voter in voters:
            current_time = time.time()
            if (current_time - start_time) > 3:
                break
            ec, _ = telosdecide.cast_vote(
                voter, ballot_name, random.choice(choices)
            )

        remaining = 5 - (current_time - start_time)
        if remaining > 0:
            time.sleep(remaining + 1)

        ec, _ = telosdecide.close_voting(ballot_name)
        assert ec == 0

        for voter in voters:
            ec, _ = telosdecide.reclaim(
                voter, 
                f'{voting_amount} {vote_symbol}',
                'test end'
            )
            assert ec == 0
            ec, _ = telosdecide.burn(
                f'{voting_amount} {vote_symbol}',
                'test end'
            )
            assert ec == 0

            
@pytest.fixture(scope="session")
def telosbookdex(eosio_testnet):
    contract = TelosBookDEX(eosio_testnet)
    yield contract

