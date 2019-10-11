#!/bin/sh

ubus call uci get "{'config':'firewall', 'type': 'rule'}" | jsonfilter -e @.values
