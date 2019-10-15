#!/bin/sh

ubus call uci get "{'config':'network', 'type': 'switch'}" | jsonfilter -e @.values
