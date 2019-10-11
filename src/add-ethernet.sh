#!/bin/sh

echo START

/home/art/Coding/zcore/src/zgen -o vid=$1 ethernet.schema | ssh root@192.168.1.1 /root/add-ethernet.sh $1

