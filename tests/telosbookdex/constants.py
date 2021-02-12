#!/usr/bin/env python3

from typing import Optional, List

import pytest

from pytest_eosiocdt import (
    collect_stdout,
    random_string,
    random_local_url
)


class TelosBookDEX:
    contract_name = 'telosbookdex'

    def __init__(self, eosio_testnet):
        self.testnet = eosio_testnet
        self.testnet.tlos_token_setup()
        ec, _ = self.testnet.push_action(
            TelosBookDEX.contract_name,
            'init',
            [],
            f'{TelosBookDEX.contract_name}@active'
        )
        assert ec == 0 

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

        ec, out = self.testnet.push_action(
            TelosBookDEX.contract_name,
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
            clients = self.testnet.get_table(
                TelosBookDEX.contract_name,
                TelosBookDEX.contract_name,
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

        ec, out = self.testnet.push_action(
            TelosBookDEX.contract_name,
            'addtnkgroup',
            [admin, title, website, brief, banner, thumbnail],
            f'{admin}@active'
        )

        if ec == 0:
            # get group id
            groups = self.testnet.get_table(
                TelosBookDEX.contract_name,
                TelosBookDEX.contract_name,
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

        return self.testnet.push_action(
            TelosBookDEX.contract_name,
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
        return self.testnet.push_action(
            TelosBookDEX.contract_name,
            'chnggroups',
            [sym, groups],
            f'{admin}@active'
        )

    def set_currency(self, admin: str, sym: str, value: bool, group: int):
        return self.testnet.push_action(
            TelosBookDEX.contract_name,
            'setcurrency',
            [sym, value, group],
            f'{admin}@active'
        )

    def get_token(self, symbol: str):
        tokens = self.testnet.get_table(
            TelosBookDEX.contract_name,
            TelosBookDEX.contract_name,
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
            TelosBookDEX.contract_name,
            quantity,
            'deposit'
        )

    def place_order(
        self,
        owner: str,
        _type: str,
        total: str,
        price: str,
        client_id: int
    ):
        return self.testnet.push_action(
            TelosBookDEX.contract_name,
            'order',
            [
                owner,
                _type,
                total,
                price,
                client_id
            ],
            f'{owner}@active'
        )


@pytest.fixture(scope="session")
def telosbookdex(eosio_testnet):
    contract = TelosBookDEX(eosio_testnet)
    yield contract

