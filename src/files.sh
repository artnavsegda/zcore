#!/bin/sh

ssh root@192.168.1.1 ubus call uci get '"'{\'config\':\'storage\', \'type\': \'file\'}'"'

