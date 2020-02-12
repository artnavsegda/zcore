#!/bin/sh

. /etc/zenith/script/lib/utils.sh
. /usr/share/libubox/jshn.sh

_COMMAND=$1;     shift
_SCHEMA=$1;      shift
_SUBSYSTEM=$1;   shift

get() {return 0};
add() {return 0};
apply() {return 0};
commit() {return 0};
revert() {return 0};
delete() {return 0};

## process global commands
case "$_COMMAND" in 
    "commit-config")
        configs=$(uci changes | awk -F. '/^[-a-z]/{print $1}' | sed -e 's/^-//g' | uniq)
        for cfg in $configs; do
            uci -q commit $cfg
        done
        stderr "all configs saved"
        exit 0
        ;;
    "revert-config")
        configs=$(uci changes | awk -F. '/^[-a-z]/{print $1}' | sed -e 's/^-//g' | uniq)
        for cfg in $configs; do
            uci -q revert $cfg
        done
        stderr "all configs reverted"
        exit 0
        ;;
esac

[ -f "/etc/zenith/script/subsystem/${_SUBSYSTEM}.sys" ] && . /etc/zenith/script/subsystem/${_SUBSYSTEM}.sys || {
    stderr "${_SUBSYSTEM} handler not found"
    exit 1
}

case "$_COMMAND" in
    add)    
        pp=""
            for key in $_CUE; do
                [ -z "$1" ] && break
                val=$1; shift
                [ -z "$val" ] && continue
                pp="$pp -o $key=$val"
            done

        DATA=$(zgen $pp $_SCHEMA)

        json_load "$DATA" || {
            stderr "data for schema corrupted"
            exit 1
        }

        ## params validation
        [ -n "$_CUE" ] && {
            # stderr "validate CUE: '$_CUE' [$pp]"
            json_get_vars $_CUE
            for key in $_CUE; do
                [ -z "$(eval echo \$$key)" ] && {
                    stderr "usage: add $_CUE"
                    exit 1
                }
            done    
        }

        add
    ;;

    get)    get $@;;

    apply)  apply $@ ;;
    
    onset)  
            _SECTION=$1; shift
            _OPTION=$1; shift
            _VALUE="$@"
            onset 
        ;;
    
    commit) commit $@ ;;
    delete) delete $@ ;;
    revert) revert $@ ;;
    
    *)      stderr "unknown command"
            exit 1
        ;;
esac

[ "$?" = "0" ] && stderr "loaded ${_SCHEMA} ${_SUBSYSTEM} | $_COMMAND"