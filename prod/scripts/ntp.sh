#!/bin/sh

ubus call uci get "{'config':'system', 'section': 'ntp'}"
