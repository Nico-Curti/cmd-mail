#!/bin/bash

function smail
{
    TO=$1;
    FROM=$2;
    cmd=$3;

    add=false;

    declare -A to_info;
    declare -A from_info;

    while read -r line
    do
        if echo "$line" | grep -q "$TO:"; then add=true; fi;
        if $add; then
            key=$(echo $line | cut -d':' -f 1);
            key="$(echo -e "${key}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')" #trim string
            value=$(echo $line | cut -d':' -f 2);
            value="$(echo -e "${value}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')" #trim string
            to_info+=([$key]=$value);
        fi
        if echo "$line" | grep -q "port"; then add=false; fi
    done < "mail.config"

    while read -r line
    do
        if echo "$line" | grep -q "$FROM:"; then add=true; fi;
        if $add; then
            key=$(echo $line | cut -d':' -f 1);
            key="$(echo -e "${key}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')" #trim string
            value=$(echo $line | cut -d':' -f 2);
            value="$(echo -e "${value}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')" #trim string
            from_info+=([$key]=$value);
        fi
        if echo "$line" | grep -q "port"; then add=false; fi
    done < "mail.config"

    subject="Results run $cmd";
    
    echo -n "Password ${from_info["address"]}:" 
    read -s password;
    echo

    myLog="./log.txt"
    msg="$($cmd > $myLog 2>&1)"

    mail -s "$subject" -r "${from_info["address"]}" -S smtp="${from_info["smtp"]}"\
                              -S smtp-auth=login \
                              -S smtp-auth-user="${from_info["address"]}" \
                              -S smtp-auth-password="$password" \
                              -S sendwait \
                              "$myLog" "${to_info["address"]}" <<< "$msg"
}

smail "to" "from" "ls"
