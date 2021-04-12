#!/bin/bash

main()
{
	CC=gcc
	CXX=g++

	if [[ "$*" =~ "--help" ]]; then
		help
		exit 0;
	fi

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
		("gcc"|"g++")	CC=gcc CXX=g++ ;;
		("clang|llvm")	CC=clang CXX=clang++ ;;
		*)
		;;
	esac

	rm -rf build
	mkdir build
	pushd build &> /dev/null
	CC=$CC CXX=$CXX cmake ${args[@]} ..
	make ${jobs}
	popd > /dev/null
}

help()
{
	printf "Usage: ./build.sh <options>\n\n"
	printf "  -c= | --compiler=<icc|gcc|clang> - Select preferred C/C++ compiler to build\n"
	printf "  -j= | --jobs=<N>                 - Specifies the number of jobs (commands) to run simultaneously (For faster building)\n\n"
}

# Initialize
main $*

# Exit normally
exit 0
