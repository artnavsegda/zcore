#!/bin/sh

type=$1; shift
schema=$1; shift

pp=""
	for key in $CUE; do
		[ -z "$1" ] && break
		val=$1; shift
		[ -z "$val" ] && continue
		pp="$pp -o $key=$val"
	done

logger -t "zcore" "zgen $type $schema $pp"

