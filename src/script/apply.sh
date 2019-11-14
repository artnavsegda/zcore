#!/bin/sh

SETARRAY=$(cat)

subsystem=$1
schema=$2

logger -t "zenith" "apply: $@ | $SETARRAY"

#uci -q delete network.$1
#uci -q set network.$1=interface
#ubus call uci set "{\"config\":\"network\", \"section\":\"$1\", \"type\":\"interface\", \"values\":$SETARRAY }"


