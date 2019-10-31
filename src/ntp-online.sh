#!/bin/sh

ssh root@192.168.1.1 /root/ntp.sh | /home/art/Coding/zcore/src/zfilter ntp
