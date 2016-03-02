#!/bin/bash

#check number of arguments

if [ $# -ne 3 ]; then
	echo "ERROR: not enough arguments"
	echo "USAGE:"
	echo ""
	echo "./changeSource.sh <MATRIX SIZE> <NUMBER OF THREADS> <NUMBER OF RESOLUTION LOOPS>"
	echo ""
	echo "MATRIX SIZES: 16 32 64 128 256 512 1024"
	echo "NUMBER OF THREADS: 1 2 4 8 16 32"

	exit 1
fi

#check for proper inputs
if [ $2 -gt $1 ]; then
	echo "ERROR: number of threads greater than matrix size"
	echo ""

	exit 1
fi

echo -en '\E[37;44m'"\033[1mBEFORE:\033[0m"
echo ""
cat	src/main.c | head -10

cat src/main.c	| sed 's/.*#define MATRIX_SIZE.*/#define MATRIX_SIZE '$1'/' \
				| sed 's/.*#define NUM_THREADS.*/#define NUM_THREADS '$2'/' \
				| sed 's/.*#define LOOPS.*/#define LOOPS '$3'/' > src/main.c

echo -en '\E[37;44m'"\033[1mAFTER:\033[0m"
echo ""
cat	src/main.c | head -10
