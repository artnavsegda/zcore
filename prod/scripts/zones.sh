#!/bin/sh

ubus call uci get "{'config':'firewall', 'type': 'zone'}" | jsonfilter -e @.values
