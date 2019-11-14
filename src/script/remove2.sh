#!/bin/sh

ssh root@192.168.1.1 ubus call uci delete '"'{\'config\':\'$1\',\'type\': \'$2\', \'match\':{\'name\':\'$3\'}}'"'
ssh root@192.168.1.1 /etc/init.d/firewall reload

