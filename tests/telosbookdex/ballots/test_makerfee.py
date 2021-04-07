#!/usr/bin/env python3

from pytest_eosiocdt.telos import telosdecide

from ..constants import telosbookdex


def test_ballot_on_makerfee_yes(telosdecide, telosbookdex):
    
    new_fee = '0.00900000 FEE'

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info
        ec, out = telosbookdex.dao_makerfee(
            new_fee,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert config['maker_fee'] == new_fee


def test_ballot_on_makerfee_no(telosdecide, telosbookdex):
    
    old_fee = telosbookdex.get_config()['maker_fee']
    new_fee = '0.00900000 FEE'

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info
        ec, out = telosbookdex.dao_makerfee(
            new_fee,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert config['maker_fee'] == old_fee
