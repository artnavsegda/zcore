#!/bin/sh
echo $SECTION

ssh root@192.168.1.1 /etc/zenith/script/apply-rules2.sh $SECTION

