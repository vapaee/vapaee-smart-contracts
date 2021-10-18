#!/usr/bin/env python3

from pytest_eosio import name_to_string
from pytest_eosio.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_savetoken_yes(telosdecide, telosbookdex):
    """Generate token and approve a vote to add it to the whitelist, check
    the whitelist to confirm correct insertion
    """
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_savetoken(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    wlist_entry = next(
        (row
        for row in telosbookdex.get_whitelist()
        if row['ballot'] == name_to_string(ballot_info['id'])),
        None
    )

    assert wlist_entry is not None


def test_ballot_on_savetoken_no(telosdecide, telosbookdex):
    """Generate token and attempt a vote to add it to the whitelist (but fail),
    check the whitelist to confirm it wasn't added
    """
    sym, prec, token_acc, token_acc_id = telosbookdex.init_test_token()

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_savetoken(
            sym,
            'eosio.token',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    wlist_entry = next(
        (row
        for row in telosbookdex.get_whitelist()
        if row['ballot'] == name_to_string(ballot_info['id'])),
        None
    )

    assert wlist_entry is None
