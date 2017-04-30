#!/bin/bash

source ./init.sh 2>/dev/null

echo -e "`whoami` # `date`" > log.txt
cat log.txt
make 2>>log.txt
if [ $# -eq 0 ]; then
	cat log.txt
else
	head -n $1 log.txt 
fi