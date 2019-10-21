#!/bin/sh

ssh root@192.168.1.1 uci revert network.$1
ssh root@192.168.1.1 ifup $1
sleep 1

