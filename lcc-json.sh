#!/bin/sh

USAGE="$0 SOURCE_FILE OUTPUT_FILE"

if [ "$1" != "" ]; then
	if [ "$2" != "" ]; then
		bld/x86/linux/lcc -Wf-target=json -S $1 -o $2
		exit
	fi
fi

echo $USAGE