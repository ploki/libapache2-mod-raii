#!/bin/bash


cat "$1" | while read ; do

        FILE="$(echo $REPLY | sed -e 's,[(\[].*,,g')"
        SYM="$(echo $REPLY | sed -e 's,.*[(]\([^+]*\).*,\1,p;d')"
        OFFSET="$(echo $REPLY | sed -e 's,[^+]*+\([^)]*\).*,\1,p;d')"

        SYM_ADDR=0x"$(nm "$FILE" 2>/dev/null | awk '/ '"$SYM"'$/ { print $1 }' )"

        IP_ADDR=""
        if [ "$SYM_ADDR" != "0x" ] ; then
        
                IP_ADDR="$(echo "print hex ($SYM_ADDR + $OFFSET )" |  python)"
        fi

        if [ -n "$SYM" ] ; then
                echo "at $SYM"
        else
                echo "at ???"
        fi
        
        if [ -n "$IP_ADDR" ] ; then
                echo -e "\tsource: $(addr2line -e $FILE $IP_ADDR)"
        else
                echo -e "\tbinary: $FILE"
        fi


done | c++filt
