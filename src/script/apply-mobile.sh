#!/bin/sh

ssh root@192.168.1.1 /etc/zenith/script/apply-ethernet.sh $FACE
ssh root@192.168.1.1 ifup $FACE

