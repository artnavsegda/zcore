#!/bin/sh

ubus call uci get "{'config':'network', 'type': 'interface', 'match':{'proto':'vlan'}}" | jsonfilter -e @.values | zfilter $1
