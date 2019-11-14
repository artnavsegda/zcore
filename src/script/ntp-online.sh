#!/bin/sh

ssh root@192.168.1.1 /root/ntp2.sh | /home/art/Coding/zcore/src/zfilter $1
