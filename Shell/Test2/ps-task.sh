#!/bin/bash

username=$1

[ $# -eq 1 ] || exit 1 

count=$(ps -u $username | wc -l)

while read P_COUNT USER; do

	if [ $P_COUNT > $count ]; then
		if ! [ "$username" = "$USER" ]; then		
			echo $USER	
		fi	
	fi

done < <(ps aux | tail -n +2 | cut -d " " -f1 | sort | uniq -c)

