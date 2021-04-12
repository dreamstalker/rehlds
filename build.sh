#!/bin/bash

CC=gcc
CXX=g++

n=0
args=()
for i in "$@"
do
case $i in
	-j=*|--jobs=*)
		jobs="-j${i#*=}"
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
pushd build &> /dev/null
CC=$CC CXX=$CXX cmake ${args[@]} ..
make ${jobs}
popd > /dev/null
