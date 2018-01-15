#!/bin/bash

sed s/TARGET_FILE/$1/g <Makefile_template >Makefile

make
make clean

rm Makefile