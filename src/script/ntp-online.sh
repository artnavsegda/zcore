#!/bin/sh

ssh root@192.168.1.1 /etc/zenith/script/ntp2.sh | ./zfilter $1
