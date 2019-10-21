#!/bin/sh

ssh root@192.168.1.1 /root/zones.sh | /home/art/Coding/zcore/src/zfilter $1
