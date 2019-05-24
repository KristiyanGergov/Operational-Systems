#!/bin/bash

maxRSS=$1

if [ "$EUID" -ne 0 ]; then
	echo "You must be root in order to execute this script!"
	exit 1
fi

while read line
do
	
	uid=$(echo $line | cut -d " " -f1)
	pid=$(echo $line | cut -d " " -f2)
	rss=$(echo $line | cut -d " " -f3) 
		
	if [ $rss > $maxRSS ]; then
		ps -eo uid,pid,%mem --sort -%mem | grep ^\ $uid | head -1 | cut -d " " -f2 | xargs -I % kill -9 %
		#echo $pidToKill
		#kill -9 ${pidToKill}
	fi			

	break	

done < <(ps -eo uid,pid,rss | tail -n +2)
