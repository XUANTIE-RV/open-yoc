#!/bin/bash

defconfig_file=".defconfig"

function read_config() {
    local valid_keys=("cpu" "board" "toolchain" "rtos" "libc")
    declare -A config_values
    local output=""

    while IFS='=' read -r key value || [ -n "$key" ]; do
        key=$(echo "$key" | xargs)
        value=$(echo "$value" | xargs)

        if [[ " ${valid_keys[@]} " =~ " $key " ]]; then
            [ -n "$value" ] && config_values[$key]="$value"
        fi
    done < "$defconfig_file"

    # for k in "${valid_keys[@]}"; do
    #     if [ -n "${config_values[$k]}" ]; then
    #         output+="$k=${config_values[$k]} "
    #     fi
    # done

    # echo "${output%% }"
    echo "${config_values[$1]}"
}

function update_config() {
    local cpu_arg="$1"
    local board_arg="$2"
    local rtos_arg="$3"
    local toolchain_arg="$4"
    local libc_arg="$5"

    touch "$defconfig_file"

    cpu="${cpu_arg#*=}"
    if [ -n "$cpu" ]; then
        if grep -q '^cpu=' "$defconfig_file"; then
            sed -i "s/^cpu=.*/cpu=$cpu/" "$defconfig_file"
        else
            echo "cpu=$cpu" >> "$defconfig_file"
        fi
    fi

    board="${board_arg#*=}"
    if [ -n "$board" ]; then
        if grep -q '^board=' "$defconfig_file"; then
            sed -i "s/^board=.*/board=$board/" "$defconfig_file"
        else
            echo "board=$board" >> "$defconfig_file"
        fi
    fi

    rtos="${rtos_arg#*=}"
    if [ -n "$rtos" ]; then
        if grep -q '^rtos=' "$defconfig_file"; then
            sed -i "s/^rtos=.*/rtos=$rtos/" "$defconfig_file"
        else
            echo "rtos=$rtos" >> "$defconfig_file"
        fi
    fi

    toolchain="${toolchain_arg#*=}"
    if [ -n "$toolchain" ]; then
        if grep -q '^toolchain=' "$defconfig_file"; then
            sed -i "s/^toolchain=.*/toolchain=$toolchain/" "$defconfig_file"
        else
            echo "toolchain=$toolchain" >> "$defconfig_file"
        fi
    fi

    libc="${libc_arg#*=}"
    if [ -n "$libc" ]; then
        if grep -q '^libc=' "$defconfig_file"; then
            sed -i "s/^libc=.*/libc=$libc/" "$defconfig_file"
        else
            echo "libc=$libc" >> "$defconfig_file"
        fi
    fi
}

if [[ "$1" == "update_config" ]]; then
    shift
    update_config "$@"
elif [[ "$1" == "read_config" ]]; then
    shift
    read_config "$@"
fi
