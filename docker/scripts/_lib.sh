#!/bin/bash

# $NET, $FORCE -----------------------------
NET=" "
FORCE=false
# iterate over all parameters and parse them. If prod is found, set NET to mainnet. If force is found, set FORCE to true
for var in "$@"
do
    if [[ "$var" == "prod" ]]; then
        NET="mainnet"
    fi
    if [[ "$var" == "force" ]]; then
        FORCE=true
    fi
done
export NET=$NET
export FORCE=$FORCE
export TESTNET="--url https://testnet.telos.caleos.io "
export MAINNET="--url https://telos.caleos.i "

# $HOME -----------------------------
# Get the path of the directory containing the currently executing script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
# We need to extract the unknown path from the DIR variable dynamically
HOME="$(echo $DIR | sed 's/\(.*\)\/docker\/scripts/\1/')"
export HOME=$HOME

# COMPILE CONTRACT ----------------------
COMPILE_MAX_TRIES=0
function compile_contract {
    # Get the contract name as argument
    local contract_name="$1"

    # Create the build directory if it doesn't exist
    mkdir -p "$HOME/contracts/$contract_name/build/"

    # Create the command variable with the command to execute
    local command="eosio-cpp -abigen $HOME/contracts/$contract_name/$contract_name.cpp -o $HOME/contracts/$contract_name/build/$contract_name.wasm -I $HOME/include"
    local CURRENT_ABI=$HOME/contracts/$contract_name/build/$contract_name.abi

    # Print the command
    print_command "$command"

    # Initialize the retry counter
    local retry_count=0

    # Start the loop to try to execute the command
    while true; do
        # Try to execute the command
        if $command; then
            # If it executes successfully, print the success message with a green check icon and exit the loop
            sed -i '/____comment/d' $CURRENT_ABI
            echo -e "\e[32m\u2714 Success!\e[0m"
            break
        else
            # If it fails, increment the retry counter
            retry_count=$((retry_count + 1))

            # If the maximum number of retries is exceeded, print an error message and exit the loop
            if [[ $retry_count -gt $COMPILE_MAX_TRIES ]]; then
                echo -e "\e[31m\u2717 Error...\e[0m"
                break
            fi

            # If not, print a retry message and wait 5 seconds before trying again
            echo "Retrying..."
            sleep 1
        fi
    done
}

# PRINTS -----------------------------------
function print_title() {
    echo -e "\n\e[92m$1\e[0m\n"
    sleep 1
}
function print_subtitle() {
    echo -e "\n\e[94m$1\e[0m"
}
function print_command() {
    echo -e "\e[96m$ \e[1m$1\e[0m"
}

function print_section() {
    echo -e "\n\e[1m----------------------------------------\e[0m"
    echo -e "\e[1m$1\e[0m"
    echo -e "\e[1m----------------------------------------\e[0m"
}

# COMPARE CONTRACT ABI ----------------------
function compare_contract_abi_onchain() {
    CONTRACT=$1
    local CURRENT_ABI_FILE="$HOME/contracts/$CONTRACT/build/$CONTRACT.abi"
    local ONCHAIN_ABI_FILE=$HOME/contracts/$CONTRACT/build/${CONTRACT}_onchain.abi

    if [[ ! -f "$ONCHAIN_ABI_FILE" ]]; then
        echo "Downloading $CONTRACT ABI and WASM"
        cleos $TESTNET get abi $CONTRACT > $ONCHAIN_ABI_FILE

        # transform the current 2-space indentation to 4-space indentation
        sed -i 's/  /    /g' $ONCHAIN_ABI_FILE

        # elimitate any line containing any of the following strings:
        # "error_messages", "abi_extensions" or "action_results"
        sed -i '/error_messages\|abi_extensions\|action_results/d' $ONCHAIN_ABI_FILE    

        # finally take out the last comma ONLY in the line saying: "variants": [],
        sed -i '/"variants": \[\],/ s/.$//' $ONCHAIN_ABI_FILE
    fi

    if [[ -f "$CURRENT_ABI_FILE" ]]; then
        compare_json_files $CURRENT_ABI_FILE $ONCHAIN_ABI_FILE
    else
        echo "No current abi file found"
    fi
}

function compare_json_files() {
    local CURRENT_ABI_FILE=$1
    local ONCHAIN_ABI_FILE=$2
  
    # obtener el contenido de los archivos json
    local current_abi=$(jq -c . $CURRENT_ABI_FILE)
    local onchain_abi=$(jq -c . $ONCHAIN_ABI_FILE)

    DIFERENCES=0
    local path="_"

    compare_values $path $current_abi $onchain_abi

    if [[ $DIFERENCES -gt 0 ]]; then
        echo "Differences found: $DIFERENCES"
    else
        echo "No differences found"
    fi
}

function compare_values() {
    # obtener el contenido de los archivos json
    local path=$1
    local current_abi=$2
    local onchain_abi=$3

    # check if the values are equal, and if so, return
    if [[ $current_abi == $onchain_abi ]]; then
        # echo "$path: OK ($current_abi)"
        return
    fi
    # echo "$path"
    
    # obtener las llaves de primer nivel. cualquier error deberá ser redirigido a /dev/null
    local current_abi_keys=$(echo $current_abi | jq -c 'keys' 2> /dev/null)
    local onchain_abi_keys=$(echo $onchain_abi | jq -c 'keys' 2> /dev/null)

    # if the last command failed, that means $onchain_abi is a final value (not explorable)
    # so this is a leaf node, and we already know it's different, so print the error message
    if [[ $onchain_abi_keys == "" ]]; then
        echo "ERROR: $path: $current_abi != $onchain_abi"
        DIFERENCES=$((DIFERENCES + 1))
        return
    fi

    # iterate over the onchain_abi_keys. For each key, check if it exists in the current_abi_keys
    for key in $(echo $onchain_abi_keys | jq -r '.[]'); do
        # check if the key exists in the onchain_abi_keys
        if [[ $(echo $current_abi_keys | jq -r '.[]' | grep $key) == "" ]]; then
            # if it doesn't exist, print the error message
            echo "ERROR: $path/$key doesn't exist in the current abi"
            DIFERENCES=$((DIFERENCES + 1))
        fi
    done

    # iterate over the current_abi_keys. For each key, check if it exists in the onchain_abi_keys
    for key in $(echo $current_abi_keys | jq -r '.[]'); do

        # check if the key exists in the onchain_abi_keys
        if [[ $(echo $onchain_abi_keys | jq -r '.[]' | grep $key) == "" ]]; then
            # if it doesn't exist, print the error message
            echo "ERROR: $path/$key doesn't exist in the onchain abi"
            DIFERENCES=$((DIFERENCES + 1))
        else

            selector=".$key"
            if [[ $key =~ ^[0-9]+$ ]]; then
                selector=".[$key]"
            fi
            
            # if it exists, compare the values
            local current_key_value=$(echo $current_abi | jq -c "$selector")
            local onchain_key_value=$(echo $onchain_abi | jq -c "$selector")
            compare_values "$path/$key" $current_key_value $onchain_key_value
        fi
    done
}

# CLEOS ----------------------
function execute_once() {
    local command=$1
    local data=$2
    local account=$3
    if [[ $data != "" ]]; then
        # if data is not empty, then add the data to the command
        if $command "$data" -p $account; then
            return 0
        else
            return 1
        fi
    else
        if $command; then
            return 0
        else
            return 1
        fi
    fi
}

function execute_command() {
    # execute_command "$command" $tries
    local command=$1
    local tries=$2
    local data=$3
    local account=$4
    
    # echo $command using white color for the promt $ and yellow for the command text like this: <white>$ </white><yellow>command</yellow>
    if [[ $data != "" ]]; then
        # blue
        # echo -e "\e[97m$ \e[94m$command '$data' -p $account\e[0m"

        # yellow
        # echo -e "\e[97m$ \e[93m$command '$data' -p $account\e[0m"
        print_command "$command '$data' -p $account"
    else
        print_command "$command"
    fi
    
    
    
    # this function will retry the command $tries times.
    # if it fails, it will print an error message and exit
    for i in $(seq 1 $tries); do
        # execute the command
        # if the command fails, it will print an error message and exit

        if execute_once "$command" "$data" "$account"; then
            # if the command succeeds, print a success message and return
            echo -e "\e[32m\u2713 Success\e[0m"
            return
        else
            # if the command fails and is the last try, then print a failure (echo -e "\e[31m\u2717 Error...\e[0m") and exit
            if [[ $i == $tries ]]; then
                echo -e "\e[31m\u2717 Error...\e[0m"
                exit 3
            fi
            
            # print text "Retrying..." in gray color
            echo -e "\e[90mRetrying...\e[0m"
            sleep 0.5
        fi
    done
}

function cleos_create_account() {
    local creator=$1
    local account_name=$2
    local owner_key=$3
    local active_key=$4
    local command="cleos create account $creator $account_name $owner_key $active_key"
    execute_command "$command" 1
}

function cleos_set_account_permission() {
    local account_name=$1
    local pub_key=$2
    # local command="cleos set account permission $account_name active '$parent_permission_name' -p $account_name"
    
    local command="cleos set account permission $account_name active '{"threshold": 1,"keys": [{"key": "'$pub_key'","weight": 1}],"accounts": [{"permission":{"actor":"'$account_name'","permission":"eosio.code"},"weight":1}]}' owner -p $account_name"
    
    print_command "$command"

    cleos set account permission $account_name active '{"threshold": 1,"keys": [{"key": "'$pub_key'","weight": 1}],"accounts": [{"permission":{"actor":"'$account_name'","permission":"eosio.code"},"weight":1}]}' owner -p $account_name

    # if last command exit code is not 0, then print an error message and exit
    if [[ $? != 0 ]]; then
        echo -e "\e[31m\u2717 Error...\e[0m"
        exit 1
    else
        echo -e "\e[32m\u2713 Success\e[0m"
    fi
}

function cleos_push_action() {
    local contract_account=$1
    local action=$2
    local data=$3
    local p=$4
    local signer=$5
    # local command="cleos push action $contract_account $action \"$data\" -p $contract_account"
    local command="cleos push action $contract_account $action"
    execute_command "$command" 3 "$data" "$signer"
}

function deploy_contract() {
    local contract=$1
    # action with default value of "init"
    local action=${2:-init}
    local contract_dir="$HOME/contracts/$contract/build"
    local contract_wasm="$contract.wasm"
    local contract_abi="$contract.abi"

    local command="cleos set contract $contract $contract_dir $contract_wasm $contract_abi -p $contract"
    execute_command "$command" 2

    cleos_push_action $contract $action [] -p $contract
}


# cleos get table vapaeepayhub vapaeepayhub state
function cleos_get_table() {
    local contract=$1
    local scope=$2
    local table=$3
    local command="cleos get table $contract $scope $table"
    execute_command "$command" 1
}

export alias ll='ls -las'
export alias telosmain='cleos --url https://telos.caleos.io '
export alias telostest='cleos --url https://testnet.telos.caleos.io '
