#!/bin/sh

[ -z "$1" ] && {
	__shell=$(which bash || which sh)
	logger -t "zenith" "shell $(jsonescape -s "$(echo $__shell)")"
	eval "$__shell"
} || {
	logger -t "zenith" "shell $(jsonescape -s "$(echo $@)")"
	eval "$@"	
}

echo ""
