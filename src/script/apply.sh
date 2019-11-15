#!/bin/sh

SETARRAY=$(cat)

case "$1" in
  "ntp")
    ubus call uci set "{'config':'system', 'section':'ntp', 'values':$SETARRAY }"
  ;;
  "rules")
    ubus call uci set "{\"config\":\"firewall\", \"section\":\"$_SECTION\", \"type\":\"rule\", \"values\":$SETARRAY }"
  ;;
  "zones")
    ubus call uci set "{\"config\":\"firewall\", \"section\":\"$_SECTION\", \"type\":\"zone\", \"values\":$SETARRAY }"
  ;;
esac
