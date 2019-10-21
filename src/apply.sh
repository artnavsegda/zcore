#!/bin/sh

# cat
ssh root@192.168.1.1 stdin2arg /bin/ubus call uci set
ssh root@192.168.1.1 /etc/init.d/firewall reload

