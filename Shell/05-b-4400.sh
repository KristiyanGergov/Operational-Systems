#!/bin/bash

now="$(date +'%d-%m-%Y')"

files=$(find $1 -type f -cmin -45 | xargs -I % basename %)

dir=${2:-${now}}

if ! [ -d ${dir} ]; then
	mkdir -p ${dir}
fi

for i in ${files}
do
	cp $i ${dir}
done
