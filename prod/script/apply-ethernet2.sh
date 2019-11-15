#!/bin/sh

SETARRAY=`cat`
ubus call uci set "{\"config\":\"network\", \"section\":\"$1\", \"type\":\"interface\", \"values\":$SETARRAY }"
ifup $1

