#!/bin/sh

./zgen -o vid=$1 ethernet.schema | ssh root@192.168.1.1 /etc/zenith/script/add-ethernet.sh $1

