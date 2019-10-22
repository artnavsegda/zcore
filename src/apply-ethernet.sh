#!/bin/sh

env
echo $@
cat
exit

ssh root@192.168.1.1 /root/apply-ethernet.sh $FACE
ssh root@192.168.1.1 ifup $FACE

