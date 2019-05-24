#!/bin/bash

username=$1

if [ "$EUID" -ne 0 ]; then
	echo "Must be executed as root!"
	exit 1
fi

if [ -z "$username" ]; then	
	echo "Username parameter is required!"
	exit 2
fi

procCount=$(ps -u $username | wc -l)

echo Max $procCount

while read count user
do
	if [ "$user" = "$username" ]; then
		continue;
	fi
	
	if [ $count -gt $procCount ]; then
		echo $user $count
	fi

done < <(ps -axo user:20 | tail -n +2 | sort | uniq -c)

avg=0
count=0

while IFS=: read hours minutes seconds
do
count=$(( $count + 1))
avg=$(($avg + $seconds + $minutes * 60 + $hours * 3600))

done < <(ps -axo time | tail -n +2)

echo $(( $avg / $count ))
