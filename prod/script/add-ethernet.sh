#!/bin/sh

SETARRAY=`zgen -o vid=$1 ethernet.schema`
ubus call uci add "{\"config\":\"network\", \"name\":\"vlan$1\", \"type\":\"interface\", \"values\":$SETARRAY }"

