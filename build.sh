#!/bin/bash

CC=gcc
CXX=g++

n=0
args=()
for i in "$@"
do
case $i in
	-j=*|--jobs=*)
		jobs="${i#*=}"
		shift
	;;
	-c=*|--compiler=*)
		C="${i#*=}"
		shift
	;;
	*)
	args[$n]="$i"
	((++n))
	;;
esac
done

case "$C" in
	("intel"|"icc")	CC=icc CXX=icpc ;;
	("gcc")			CC=gcc CXX=g++ ;;
	("clang")		CC=clang CXX=clang++ ;;
	*)
	;;
esac

rm -rf build
mkdir build
pushd build
CC=$CC CXX=$CXX cmake ${args[@]} ..
make -j${jobs}
popd
