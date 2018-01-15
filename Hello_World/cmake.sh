#!/bin/bash

#USAGE: ./cmake.sh mainfile dependencies dependencies

for i; do
	VAR="$VAR $i.o"
done

sed s/TARGET_FILE/$1/g <Makefile_template >tmp

sed s/TARGET_OBJECT/"$VAR"/g <tmp >Makefile

rm tmp
