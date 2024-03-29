#!/usr/bin/env python3

from pytest_eosio import collect_stdout

from .constants import TelosProfile, telosprofile


def test_witness(telosprofile):
    """Create a profile and register a verification link, then create several
    bot accounts to witness that link, at each step check for correct table
    updates.

    Then register a fresh profile with its own verification link, and
    have the first profile witness it, the new link's score should be equal to
    the total profile score of the original profile
    """
    telosprofile.init_platforms()

    # create root profile and register link
    account_link, alias_link = telosprofile.new_profile()

    proof = telosprofile.add_link(
        alias_link,  'facebook', 'https://localhost/facebook.html'
    )

    link = telosprofile.get_link_with_proof(alias_link, proof)

    assert link is not None
    assert link['points'] == 0

    # create several accounts that witness the link
    link_id = link['id']

    bot_accounts = 5

    for i in range(bot_accounts):
        account_witness, alias_witness = telosprofile.new_profile()

        telosprofile.witness_link(alias_witness, alias_link, link_id)

        link = telosprofile.get_link_with_id(alias_link, link_id)

        assert link is not None
        assert link['points'] == (i + 1)

    # update root profile points
    telosprofile.update_profile(alias_link)

    # register new profile and link
    new_account, new_alias = telosprofile.new_profile()
    proof = telosprofile.add_link(
        new_alias,  'facebook', 'https://localhost/facebook.html'
    )

    link = telosprofile.get_link_with_proof(new_alias, proof)

    assert link is not None

    # witness this new link with root account
    link_id = link['id']
    telosprofile.witness_link(alias_link, new_alias, link_id)

    link = telosprofile.get_link_with_id(new_alias, link_id)

    wprofile = telosprofile.get_profile(alias_link)

    assert link is not None
    assert link['points'] == wprofile['points']


def test_witness_profile_not_found_witness(telosprofile):
    """Attempt to witness a link on a non existent profile, check error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        ['not a profile', 'not a profile', '0'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (witness)' in out


def test_witness_not_authorized(telosprofile):
    """Attempt to witness a link using the wrong signature, check error message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        [alias, 'not a profile', '0'],
        'eosio@active',
        retry=0
    )
    assert ec == 1
    assert 'not authorized' in out


def test_witness_profile_not_found_link(telosprofile):
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        [alias, 'not a profile', '0'],
        f'{account}@active',
        retry=0
    )
    assert ec == 1
    assert 'profile not found (link)' in out


def test_witness_link_not_found(telosprofile):
    """Attempt to witness a non existant link, check error message
    """
    waccount, walias = telosprofile.new_profile()
    laccount, lalias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'witness',
        [walias, lalias, '6920105956'],
        f'{waccount}@active',
        retry=0
    )
    assert ec == 1
    assert 'link not found' in out
