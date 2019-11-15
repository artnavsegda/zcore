#!/bin/sh

SETARRAY=$(cat)

subsystem=$1
schema=$2

logger -t "zenith" "apply: $@ | $SETARRAY"

#ubus call uci set "{\"config\":\"network\", \"section\":\"$1\", \"type\":\"interface\", \"values\":$SETARRAY }"

case "$subsystem" in
    "apn-profiles")
            ## unnamed section
            uci -q delete apn-profiles.$_SECTION ## need remove option value
            _SECTION=$(uci add apn-profiles apn)
            DATA='{"config": "apn-profiles", "section": "'$_SECTION'", "type": "apn", "values": '$SETARRAY'}'
            eval "ubus call uci set '$DATA'"
        ;;
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
