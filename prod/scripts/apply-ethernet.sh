#!/bin/sh

SETARRAY=`cat`
ubus call uci set "{\"config\":\"network\", \"section\":\"$SECTION\", \"type\":\"interface\", \"values\":$SETARRAY }"
ifup $SECTION

