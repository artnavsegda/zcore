#!/bin/sh

echo deleting $1

ssh root@192.168.1.1 ubus call uci delete '"'{\'config\':\'$1\',\'type\': \'$2\', \'match\':{\'name\':\'$3\'}}'"'
