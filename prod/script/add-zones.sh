#!/bin/sh
SETARRAY=`cat`
ubus call uci add "{\"config\":\"network\", \"name\":\"vlan$1\", \"type\":\"interface\", \"values\":$SETARRAY }"

