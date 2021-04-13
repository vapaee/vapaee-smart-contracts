#!/usr/bin/env python3

from pytest_eosiocdt import name_to_string
from pytest_eosiocdt.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_hblockprune_yes(telosdecide, telosbookdex):

    days = 90

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_hblockprune(
            days,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert int(config['kprune']) == days


def test_ballot_on_hblockprune_no(telosdecide, telosbookdex):

    old_days = int(telosbookdex.get_config()['kprune'])
    new_days = 90

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_hblockprune(
            new_days,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert int(config['kprune']) == old_days
