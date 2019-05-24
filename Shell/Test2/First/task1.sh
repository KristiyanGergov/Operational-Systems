#!/bin/bash

input=$1

if [ -d $input ]; then
	find . -xtype l
else
	echo "$1 is not a directory!"
	exit 1
fi
