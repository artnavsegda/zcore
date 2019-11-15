#!/bin/sh

echo $1

SETARRAY=`cat`
ubus call uci set "{\"config\":\"firewall\", \"section\":\"$1\", \"type\":\"rule\", \"values\":$SETARRAY }"
/etc/init.d/firewall reload
echo $SETARRAY
echo $1

