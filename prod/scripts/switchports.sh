#!/bin/sh

ubus call uci get "{'config':'network', 'type': 'switch_port'}"
