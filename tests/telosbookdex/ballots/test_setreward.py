#!/usr/bin/env python3

import pytest

from pytest_eosiocdt.telos import telosdecide

from ..constants import telosbookdex


reward_names = (
    'trademaker',
    'tradetaker',
    'delmarkets',
    'history',
    'events',
    'points',
    'ballots'
)

config_names = (
    'maker_X_reward',
    'taker_X_reward',
    'maint_reward_delmarkets_X',
    'maint_reward_history_X',
    'maint_reward_events_X',
    'maint_reward_points_X',
    'maint_reward_ballots_X'
)

reward_types = ('pts', 'exp')

params = []
for reward_type in reward_types:
    for reward_name, config_name in zip(
        reward_names, config_names
    ):
        params.append((
            reward_name,
            config_name.replace('X', reward_type),
            reward_type
        ))

@pytest.mark.parametrize(
    'reward_name,config_name,reward_type',
    [pytest.param(param[0], param[1], param[2], id=param[1])
    for param in params]
)
def test_ballot_on_setreward_yes(
    telosdecide,
    telosbookdex,
    reward_name, config_name, reward_type
):
    """Approve ballot to change reward multiplier global config setting & check
    respective table for correct update
    """
    new_rew = .75

    with telosbookdex.perform_vote(
        telosdecide,
        [['yes']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_setreward(
            reward_name,
            reward_type,
            new_rew,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 1
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert float(config[config_name]) == new_rew


@pytest.mark.parametrize(
    'reward_name,config_name,reward_type',
    [pytest.param(param[0], param[1], param[2], id=param[1])
    for param in params]
)
def test_ballot_on_setreward_no(
    telosdecide,
    telosbookdex,
    reward_name, config_name, reward_type
):
    """Attempt ballot to change reward multiplier global config setting (but
    fail) & check value stays the same
    """
    old_rew = float(telosbookdex.get_config()[config_name])
    new_rew = .75

    with telosbookdex.perform_vote(
        telosdecide,
        [['no']]
    ) as vote_info:
        voters, ballot_acc  = vote_info 
        ec, out = telosbookdex.dao_setreward(
            reward_name,
            reward_type,
            new_rew,
            ballot_acc
        )

        assert ec == 0

    ballot_info = telosbookdex.get_ballot_by_feepayer(ballot_acc) 

    assert ballot_info is not None
    assert ballot_info['approved'] == 0
    assert ballot_info['accepted'] == 1

    config = telosbookdex.get_config()

    assert float(config[config_name]) == old_rew
