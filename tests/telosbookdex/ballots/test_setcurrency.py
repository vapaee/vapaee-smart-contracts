#!/usr/bin/env python3

from pytest_eosiocdt.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_setcurrency_yes(telosdecide, telosbookdex):
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

    breakpoint()

