#!/usr/bin/env python3

from typing import Optional, List

import pytest

from pytest_eosiocdt import (
    collect_stdout,
    random_string,
    random_local_url
)
from pytest_eosiocdt.telos import init_telos_token
from pytest_eosiocdt.contract import SmartContract


def get_market_scope(
    sym_a: str,
    sym_b: str
):
    return f'{sym_a.lower()}.{sym_b.lower()}'[:12]


_did_init = False
class TelosBookDEX(SmartContract):
    
    @property
    def contract_name(self) -> str:
        return 'telosbookdex'

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

    def set_currency(self, admin: str, sym: str, value: bool, group: int):
        return self.push_action(
            'setcurrency',
            [sym, value, group],
            f'{admin}@active'
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


@pytest.fixture(scope="session")
def telosbookdex(eosio_testnet):
    contract = TelosBookDEX(eosio_testnet)
    yield contract

