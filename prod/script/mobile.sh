#!/bin/sh

ubus call uci get "{'config':'network', 'type': 'interface', 'match':{'proto':'3g'}}" | jsonfilter -e @.values

