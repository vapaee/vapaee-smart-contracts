#!/usr/bin/env python3

from pytest_eosiocdt import collect_stdout

from .constants import TelosProfile, telosprofile


def test_addlink(telosprofile):
    telosprofile.init_platforms()
    account, alias = telosprofile.new_profile()

    url = 'https://localhost/facebook.html'
    proof = telosprofile.add_link(alias, 'facebook', url)
    assert len(proof) == 12

    link = telosprofile.get_link_with_proof(alias, proof)

    assert link is not None
    assert link['url'] == url


def test_addlink_profile_not_found(telosprofile):
    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        ['not an alias', 'facebook', 'https://localhost/facebook.html'],
        f'eosio@active'
    )
    assert ec == 1
    assert 'profile not found' in out


def test_addlink_not_authorized(telosprofile):
    telosprofile.init_platforms()
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'parler', 'https://localhost/parler.html'],
        'eosio@active'
    )
    assert ec == 1
    assert 'not authorized' in out


def test_addlink_platform_not_found(telosprofile):
    account, alias = telosprofile.new_profile()

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'parler', 'https://localhost/parler.html'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'platform not found' in out


def test_addlink_already_exists(telosprofile):
    telosprofile.init_platforms()
    account, alias = telosprofile.new_profile()

    telosprofile.add_link(
        alias,  'facebook', 'https://localhost/facebook.html'
    )

    ec, out = telosprofile.testnet.push_action(
        TelosProfile.contract_name,
        'addlink',
        [alias, 'facebook', 'https://localhost/facebook.html'],
        f'{account}@active'
    )
    assert ec == 1
    assert 'link for this platform already exists' in out