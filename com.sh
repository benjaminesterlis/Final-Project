#!/bin/bash
make >& log.txt
head -n $1 log.txt 
