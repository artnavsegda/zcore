#!/bin/sh
SETARRAY=`cat | jq -c 'del(.[".index", ".anonymous",".type",".name"])'`
ubus call uci set "{\"config\":\"network\", \"section\":\"$1\", \"type\":\"interface\", \"values\":$SETARRAY }"

