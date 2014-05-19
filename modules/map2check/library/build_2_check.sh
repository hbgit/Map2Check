#!/bin/bash

#usage ./script new_cprogram.c

CC=gcc
DEPS=check_safety_memory_FORTES.h
LIBRARY=check_safety_memory_FORTES.o

if [ $# -gt 0 ];
then	
	#Compile the new program
	#echo "File C: "$1 
	$CC -c $1 

	#Get the name.o from new C program  
	rec_program_path_O=$(echo $1 | sed -e 's/.c$/.o/') 	
	rec_program_path_exec=$(echo $1 | sed -e s'/.c$/_2check/') 
	
	#$2 -> addtional options to compile
	$CC $LIBRARY $rec_program_path_O -o $rec_program_path_exec $2
	
	rm $rec_program_path_O

else
	echo "Usage way: ./script < C source code mapped >"
fi
