#!/bin/bash
date
make 2>log.txt
if [ $# -eq 0 ]; then
	cat log.txt
else
	head -n $1 log.txt 
fi
