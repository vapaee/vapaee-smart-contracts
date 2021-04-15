#!/usr/bin/env python3

from pytest_eosiocdt.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_setcurrency_yes_non_currency(telosdecide, telosbookdex):
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info
        ec, out = telosbookdex.dao_setcurrency(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    token_groups = telosbookdex.get_token_groups()

    assert token_groups is not None
    assert sym in token_groups[0]['currencies']


def test_ballot_on_setcurrency_no_non_currency(telosdecide, telosbookdex):
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info
        ec, out = telosbookdex.dao_setcurrency(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    token_groups = telosbookdex.get_token_groups()

    assert token_groups is not None
    assert sym not in token_groups[0]['currencies']


def test_ballot_on_setcurrency_yes_currency(telosdecide, telosbookdex):
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    # has to previously be a currency
    ec, _ = telosbookdex.set_currency(sym, True, 0)
    assert ec == 0

    token_groups = telosbookdex.get_token_groups()

    assert token_groups is not None
    assert sym in token_groups[0]['currencies']

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info
        ec, out = telosbookdex.dao_setcurrency(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    token_groups = telosbookdex.get_token_groups()

    assert token_groups is not None
    assert sym in token_groups[0]['currencies']


def test_ballot_on_setcurrency_no_currency(telosdecide, telosbookdex):
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    # has to previously be a currency
    ec, _ = telosbookdex.set_currency(sym, True, 0)
    assert ec == 0

    token_groups = telosbookdex.get_token_groups()

    assert token_groups is not None
    assert sym in token_groups[0]['currencies']

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info
        ec, out = telosbookdex.dao_setcurrency(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    token_groups = telosbookdex.get_token_groups()

    assert token_groups is not None
    assert sym not in token_groups[0]['currencies']
