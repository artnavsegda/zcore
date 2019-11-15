#!/bin/sh

. /usr/share/libubox/jshn.sh

subsystem=$1; shift
schema=$1; shift

pp=""
    for key in $CUE; do
        [ -z "$1" ] && break
        val=$1; shift
        [ -z "$val" ] && continue
        pp="$pp -o $key=$val"
    done

logger -t "zcore-add" "$subsystem - zgen $pp $schema"

DATA=$(zgen $pp $schema)

json_load "$DATA" || {
    echo "DATA corrupted"
    exit 1
}

case "$subsystem" in
    "apn-profiles")
        json_get_vars $CUE
        for key in $CUE; do
            [ -z "$(eval echo \$$key)" ] && {
                echo "usage: add <$CUE>"
                exit 1
            }
        done

        config='{"config": "apn-profiles", "type": "apn", "values": '$(json_dump)'}'
        eval "ubus call uci add '$config'" &> /dev/null

        ;;
    "zones")
        ;;
    "rules")
        ;;
    "ethernet")
        json_get_vars $CUE
        for key in $CUE; do
            [ -z "$(eval echo \$$key)" ] && {
                echo "usage: add <$CUE>"
                exit 1
            }
        done

        config='{"config": "apn-profiles", "type": "apn", "values": '$(json_dump)'}'
        eval "ubus call uci add '$config'" &> /dev/null

        ;;
esac
