#!/bin/bash
if [ $# != 1 ]
	then
		printf "syntax error\ncompile.sh [programName (without .c)]\n"
	else
		gcc -c $1.c
		gcc -o $1 $1.o
		rm -f $1.o
fi