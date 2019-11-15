#!/bin/sh

ssh root@192.168.1.1 /etc/zenith/script/mobile.sh | /home/art/Coding/zcore/src/zfilter $1
