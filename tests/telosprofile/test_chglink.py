#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile, telosprofile


def test_chglink(telosprofile):
    """Create new profile & link, then change its URL, check tables for correct
    update
    """
    telosprofile.init_platforms()
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 0
    proof_token = collect_stdout(out)

    link = telosprofile.get_link_with_proof(alias, proof_token)
    assert link is not None

    link_id = link['id']

    new_url = 'https://localhost/facebook2.html'

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        [alias, str(link_id), new_url],
        f'{account}@active'
    )
    assert ec == 0

    link = telosprofile.get_link_with_id(alias, link_id)

    assert link is not None
    assert link['url'] == new_url


def test_chglink_profile_not_found(telosprofile):
    """Attempt to change link of a non existent profile, check for correct
    error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        ['not a profile', '0', 'localhost'],
        'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_chglink_profile_not_authorized(telosprofile):
    """Attempt to change link using wrong signature, check for correct error
    message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        [alias, '0', 'localhost'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_chglink_link_not_found(telosprofile):
    """Attempt to change a non existent link, check for correct error message
    """
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'chglink',
        [alias, '0', 'localhost'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'link not found' in out
