#!/bin/sh

logger -t "zenith" "get: $@"

case "$1" in
	"dns")
		x=$(ubus call uci get "{'config': 'dhcp', 'type': 'dnsmasq'}" | jsonfilter -e @.values[*].server)
		echo '{"servers": '$x'}' | zfilter $2
	;;

    "wifi-phy")
        ubus call uci get "{'config': 'wireless', 'type': 'wifi-device'}"  | jsonfilter -e @.values | zfilter $2
    ;;

    "wifi-iface")
        ubus call uci get "{'config': 'wireless', 'type': 'wifi-iface'}"  | jsonfilter -e @.values | zfilter $2
    ;;


	"mobile")
		ubus call uci get "{'config':'network', 'type': 'interface', 'match':{'proto':'3g'}}" | jsonfilter -e @.values | zfilter $2
	;;
	
	"apn-profiles")
        	ubus call uci get "{'config': 'apn-profiles', 'type': 'apn'}" | jsonfilter -e "@.values" | zfilter $2
	;;

    "loopback")
        ubus call uci get "{'config': 'network', 'type': 'interface', 'match': {'ifname': 'lo'}}" | jsonfilter -e @.values | zfilter $2
    ;;

    "routes")
        ubus call uci get "{'config': 'network', 'type': 'route'}" | jsonfilter -e @.values | zfilter $2
    ;;

	"lan")
        local firstPass=1

        rr=$(echo '{'
            for interface in $(uci show network | awk -F'=' '/network.lan[0-9]{0,4}=/{print $1}'); do
                id=$(uci -q get ${interface}.ifname | awk -F'.' '{print $2}')
                [ -z "$id" ] && continue
                uci -q get network.vlan_${id} &> /dev/null || continue
                cpu_port=$(uci -q get network.vlan_${id}.cpu_port)
                ports=$(uci -q get network.vlan_${id}.ports | awk '{gsub("[a-zA-Z]",""); sub("'$cpu_port'",""); sub("\ $",""); print}')
                ip=$(uci -q get $interface.ipaddr)
                mask=$(uci -q get $interface.netmask)
                macaddr=$(uci -q get $interface.macaddr)
                name=${interface#*.}

                [ $firstPass = 0 ] && echo ","
                echo -n '"'$name'": {
                            "vid": '$id',
                            "ports": '$(jsonescape -l $ports)',
                            "cpu_port":     '$cpu_port',
                            "ipaddr":       '$(jsonescape -s "$ip")',
                            "netmask":      '$(jsonescape -s "$mask")',
                            "macaddr":      '$(jsonescape -s "$macaddr")',
                            "mtu":          '$(uci -q get $interface.mtu || echo 1500)',
                            "bridge":       '$([ `uci -q get $interface.type` = "bridge" ] && echo 1 || echo 0)',
                            "stp":          '$(uci -q get $interface.stp || echo 0)'}'
                firstPass=0

            done
            echo '}')

        echo $rr | zfilter $2		
	;;

	"wan")
        local firstPass=1

        rr=$(echo '{'

        for interface in $(uci show network | awk -F'=' '/network.wan[0-9]{0,4}=/{print $1}'); do
            id=$(uci -q get ${interface}.ifname | awk -F'.' '{print $2}')
            [ -z "$id" ] && continue
            uci -q get network.vlan_${id} &> /dev/null || continue
            cpu_port=$(uci -q get network.vlan_${id}.cpu_port)
            ports=$(uci -q get network.vlan_${id}.ports | awk '{gsub("[a-zA-Z]",""); sub("'$cpu_port'",""); sub("\ $",""); print}')

            peerdns=$(uci -q get ${interface}.peerdns)
            [ -z $peerdns ] && peerdns=1

                         ip=$(jsonescape -s "$(uci -q get ${interface}.ipaddr)")
                       mask=$(jsonescape -s "$(uci -q get ${interface}.netmask)")
                         gw=$(jsonescape -s "$(uci -q get ${interface}.gateway)")
                    peerdns=$(jsonescape -b "$peerdns")
               defaultroute=$(jsonescape -b "$(uci -q get ${interface}.defaultroute)")
                      proto=$(jsonescape -s "$(uci -q get ${interface}.proto)")
                     metric=$(jsonescape -s "$(uci -q get ${interface}.metric)")
                        mac=$(jsonescape -s "$(uci -q get ${interface}.macaddr)")
                    enabled=$(jsonescape -b "$(uci -q get ${interface}.auto)")
               pingInterval=$(jsonescape -s "$(uci -q get ${interface}.ping_intvl)")
                pingAddress=$(jsonescape -s "$(uci -q get ${interface}.ping_ip)")
                pingMaxfail=$(jsonescape -s "$(uci -q get ${interface}.ping_maxfail)")
                 pppoeLogin=$(jsonescape -s "$(uci -q get ${interface}.username)")
              pppoePassword=$(jsonescape -s "$(uci -q get ${interface}.password)")
                    pppoeAC=$(jsonescape -s "$(uci -q get ${interface}.ac)")

            name=${interface#*.}

            [ $firstPass = 0 ] && echo ","
            echo -n '"'$name'": {
                "vid": '$id',
                "ports": '$(jsonescape -l $ports)',
                "cpu_port": '$cpu_port',
                "ipaddr": '$ip',
                "netmask": '$mask',
                "gateway": '$gw',
                "peerdns": '$peerdns',
                "defaultroute": '$defaultroute',
                "proto": '$proto',
                "macaddr": '$mac',
                "enable": '$enabled',
                "username": '$pppoeLogin',
                "password": '$pppoePassword',
                "ac-name": '$pppoeAC',
                "metric": '$metric',
                "ping-addr": '$pingAddress',
                "ping-fail": '$pingMaxfail',
                "ping-interval": '$pingInterval'
            }'

            firstPass=0
        done
        echo '}')

        echo $rr | zfilter $2
	;;

    "switch-ports")

        rr=$(echo '{'
        . /usr/share/libubox/jshn.sh

        x=$(ubus call uci get "{'config': 'network', 'type': 'switch_port', 'match': {'to_cpu': 0}}")
        json_load "$x"
        json_select values
        json_get_keys ports &> /dev/null
        for p in $ports; do
        json_select $p

        json_get_var name name
        json_get_var link link
        json_get_var port port
        json_get_var port_disabled port_disabled
        status=$(swconfig dev switch0 port $port get link | sed -E s'/port\:[0-9]+\ //g')

        [ -n "$link" ] && eval $( echo $link | awk '{ for (i=1;i<=NF;i+=2) printf("%s=%s\n",$i,$(i+1)) }' ) &> /dev/null
        [ -z "$speed" ] && speed=auto
        [ -z "$duplex" ] && duplex=full
        [ -z "$port_disabled" ] && port_disabled=0

        [ "$speed" = "0" ] && speed=auto
        [ "$status" = "link:down" -a "$port_disabled" = "1" ] && status="disabled"

        [ "$i" = "1" ] && echo ","
	echo -n "\"$p\": {\"name\": \"$name\", \"port\": $port, \"speed\": \"$speed\", \"duplex\": \"$duplex\", \"status\": \"$status\", \"disabled\": $port_disabled}"

        i=1
        json_select ..
        done

        echo '}')

        echo $rr | zfilter $2
    ;;

	"switch-master-ports")
		ubus call uci get "{'config':'network', 'type': 'switch_port', 'match':{'to_cpu': 1}}" | jsonfilter -e @.values | zfilter $2
	;;

esac

