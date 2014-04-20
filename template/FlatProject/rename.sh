#!/bin/bash

if [ ! -e rename.sh ] ; then
	echo "$0 must be called from source directory">&2
	exit 1
fi

NAME="$1"

if [ -z "$NAME" ] ; then
	echo "a project name must be provided">&2
	exit 1
fi

find . -type f | while read ; do
	FILE="$REPLY"
	if [ "$FILE" = "./rename.sh" ] ; then
		continue
	fi
	perl -pi -e "s/FlatProject/$NAME/g" "$FILE"
	if echo "$FILE" | grep FlatProject >/dev/null ; then
		NEWFILE="$(echo "$FILE" | sed -e "s/FlatProject/$NAME/g"  )"
		mv "$FILE" "$NEWFILE"
	fi
done


