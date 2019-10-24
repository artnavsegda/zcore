#!/bin/sh

SETARRAY=`cat`
ubus call uci set "{\"config\":\"firewall\", \"section\":\"$FACE\", \"type\":\"rule\", \"values\":$SETARRAY }"
/etc/init.d/firewall reload
echo $SETARRAY
echo $FACE

