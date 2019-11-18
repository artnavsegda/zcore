#!/bin/sh

echo deleting $1
ubus call uci delete "{\"config\":\"$1\", \"section\": \"$3\",\"type\": \"$2\" }"
