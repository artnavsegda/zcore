#!/bin/sh

echo $1 $2 $3
echo sobake
echo $@

SETARRAY=`cat`
ubus call uci set "{\"config\":\"network\", \"section\":\"$FACE\", \"type\":\"interface\", \"values\":$SETARRAY }"
ifup $FACE

