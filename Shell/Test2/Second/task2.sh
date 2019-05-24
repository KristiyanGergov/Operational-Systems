#!/bin/bash

fileName=$1
firstValue=$2
secondValue=$3

if [ -z $fileName ]; then echo "File name not provided!"; fi
if [ -z $firstValue ]; then echo "First value not provided!"; fi
if [ -z $secondValue ]; then echo "Second value not provided!"; fi

firstLine=$(cat $fileName | grep ^$firstValue= | cut -d '=' -f2)
secondLine=$(cat $fileName | grep ^$secondValue= | cut -d '=' -f2)

newLine=$secondLine
for arg in $firstLine
do
	newLine=$(echo $newLine | sed -r "s/${arg}//g")
done

newLine=$(echo $newLine | sed "s/^/$secondValue=/g")
secondLine=$(cat $fileName | grep ^$secondValue=)

cat $fileName | sed "s/${secondLine}/${newLine}/g"
