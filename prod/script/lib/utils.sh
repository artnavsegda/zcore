. /usr/share/libubox/jshn.sh

stderr() {
    echo "$@" >&2
}

keymatch() {
    ## keymatch variable keys values pattern
    local _var=$1; shift
    local _keys=$1; shift
    local _vals=$1; shift
    local _pattern=$1; shift
    
    local keynumber=-1
    local valuenumber
    local k v

    for k in $_keys; do
        let keynumber++
        [ "$k" = "$_pattern" ] || continue
        let valuenumber=0
        for v in $_vals; do
            if [ "$keynumber" = "$valuenumber" ]; then
                eval export "$_var=${v}"
                return 0
            fi
            let valuenumber++
        done
    done
    eval export "$_var=''"
    return 1
}

set_values() {
    local _cfg=$1
    local _obj=$2

    json_load "$3"
    json_get_keys keys
    json_get_vars $keys

    for k in $(uci -X show ${_cfg}.${_obj} | awk '{split($0, x, "."); split(x[3], y, "="); print y[1]}'); do
        [ -z "$k" ] && continue
        uci -q delete ${_cfg}.${_obj}.$k
    done

    for key in $keys; do
        uci -q set ${_cfg}.${_obj}.$key="$(eval echo \$$key)"
    done

}

get_random_hex(){
    ## get_random_hex <variable> [prefix length]
    local _var=$1
    local _prefix=${2:-cfg_}
    local l=${3:-6}

    [ $l -gt 32 ] && l=32

    ss="${_prefix}$(head -c 32 /dev/urandom | md5sum | head -c $l)"
    eval export "$_var=${ss}"
}

get_next_name() {
    ## get_next_name <variable> [config prefix_name]
    local _var=$1
    local _cfg=$2
    local _name=$3

    let x=1
    nums=$(uci -X show ${_cfg} | grep -Eo "${_cfg}.${_name}[0-9]{0,4}" | sort -u | sed s"/${_cfg}.${_name}//" | sort -n)
    for num in $nums; do
        [ -z $num ]     && continue
        [ $num -lt 1 ]  && continue
        [ $num -eq $x ] && {
            let x++
            continue
        }
        [ $num -lt $x ] && {
            eval export "$_var=${_name}${x}"
            return 0
        }

    done
    eval export "$_var=${_name}${x}"
}