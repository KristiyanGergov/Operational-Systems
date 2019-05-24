#!/bin/bash

[ "$EUID" -ne 0 ] && exit 1

homeDirs=$(cat /etc/passwd | cut -d: -f6)

mostRecent="0"

while read directory
do

	if ! [ -d $directory ]; then
		continue
	fi	

	current=$(find $directory -type f -exec stat --format '%Y :%y %n' "{}" \; | sort -nr | cut -d: -f2- | head -1)	

	if [ "$current" \> "$mostRecent" ]; then
		mostRecent=$current
	fi

done < <(cat /etc/passwd | cut -d: -f6)

echo $mostRecent
