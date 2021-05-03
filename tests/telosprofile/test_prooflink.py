#!/usr/bin/env python3

from .constants import TelosProfile, telosprofile


def test_prooflink(telosprofile):
    """Create a profile & add social media link, then set verification link,
    check tables for correct updates
    """
    telosprofile.init_platforms()
    account, alias = telosprofile.new_profile()

    proof = telosprofile.add_link(
        alias,
        'steemit',
        'steemit.io/profiles.html?user=mario'
    )
    assert len(proof) == 12

    link = telosprofile.get_link_with_proof(alias, proof)
    link_id = link['link_id']

    proof_url = 'steemit.io/profiles.html?user=mario?postid=06283688886909'

    ec, _ = telosprofile.set_proof_link(
        alias,
        link_id,
        proof_url 
    )

    assert ec == 0

    link = telosprofile.get_link_with_id(alias, link_id)

    assert link['points'] == 0
    assert link['proof'] == proof_url
    assert len(link['witnesses']) == 0


def test_prooflink_profile_not_found(telosprofile):
    """Attempt to set proof link of a non existent profile, check for correct
    error message
    """
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'prooflink',
        ['not an alias', '0', 'https://localhost/facebook.html'],
        f'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_prooflink_not_authorized(telosprofile):
    """Attempt to set proof link using the wrong signature, check for correct
    error message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'prooflink',
        [alias, '0', 'https://localhost/facebook.html'],
        f'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_prooflink_link_not_found(telosprofile):
    """Attempt to set proof link of a non existent link, check for correct
    error message
    """
    account, alias = telosprofile.new_profile()
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'prooflink',
        [alias, '0', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'link not found' in out
