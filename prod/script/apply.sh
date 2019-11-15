#!/bin/sh

SETARRAY=$(cat)

case "$1" in
  "ntp")
    ubus call uci set "{'config':'system', 'section':'ntp', 'values':$SETARRAY }"
  ;;
esac

#subsystem=$1
#schema=$2

#logger -t "zenith" "apply: $@ | $SETARRAY"

#uci -q delete network.$1
#uci -q set network.$1=interface
#ubus call uci set "{\"config\":\"network\", \"section\":\"$1\", \"type\":\"interface\", \"values\":$SETARRAY }"
