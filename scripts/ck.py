#!/usr/bin/env python3

import json

from pathlib import Path


abis = {}

for abi_path in Path('contracts').glob('**/*.abi'):
    with open(abi_path, 'r') as abi_file:
        abi = json.loads(abi_file.read())

        contract_name = abi_path.name
        abis[contract_name] = abi


for contract, abi in abis.items():
    print(f'{contract}:')
    for action in [a['name'] for a in abi['actions']]:
        for sub_contract, sub_abi in abis.items():
            if sub_contract == contract:
                continue

            if action in [a['name'] for a in sub_abi['actions']]:
                print(f'\t - {action} in {sub_contract}')


