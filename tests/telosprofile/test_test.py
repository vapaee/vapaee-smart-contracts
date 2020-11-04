
def test_tprofile_test2(eosio_testnet):
    ec, out = eosio_testnet.push_action(
        'telosprofile',
        'test2',
        [],
        'telosprofile@active'
    )
    assert ec == 0