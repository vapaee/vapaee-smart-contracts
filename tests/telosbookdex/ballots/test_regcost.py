#!/usr/bin/env python3

from pytest_eosio import name_to_string
from pytest_eosio.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_regcost_yes(telosdecide, telosbookdex):
    """Approve ballot to change regcost global config setting & check
    respective table for correct update
    """
    new_cost = 420

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_regcost(
            f'{new_cost} TLOS',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert config['regcost'] == f'0.{new_cost} TLOS'


def test_ballot_on_regcost_no(telosdecide, telosbookdex):
    """Attempt ballot to change regcost global config setting (but fail) &
    check value stays the same
    """
    old_cost = telosbookdex.get_config()['regcost']
    new_cost = 420

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_regcost(
            f'{new_cost} TLOS',
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert config['regcost'] == old_cost
