#!/bin/sh

ssh root@192.168.1.1 /root/add-ethernet.sh $1
ssh root@192.168.1.1 ifup $1

