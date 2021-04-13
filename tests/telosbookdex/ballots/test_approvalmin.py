#!/usr/bin/env python3

from pytest_eosiocdt import name_to_string
from pytest_eosiocdt.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_approvalmin_yes(telosdecide, telosbookdex):

    new_min = .5

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_approvalmin(
            new_min,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert float(config['approvalmin']) == new_min


def test_ballot_on_approvalmin_no(telosdecide, telosbookdex):

    old_min = float(telosbookdex.get_config()['approvalmin'])
    new_min = .5

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_approvalmin(
            new_min,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert float(config['approvalmin']) == old_min
