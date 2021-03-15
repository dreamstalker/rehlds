cmake_minimum_required(VERSION 3.1)
project(bzip2 C)

set(BZIP2_SRCS
	"src/blocksort.c"
	"src/bzlib.c"
	"src/compress.c"
	"src/crctable.c"
	"src/decompress.c"
	"src/huffman.c"
	"src/precompiled.c"
	"src/randtable.c"
	"src/bzlib_private.h"
)

include_directories(
	"include"
)

add_library(bzip2 STATIC ${BZIP2_SRCS})
set_target_properties(bzip2 PROPERTIES COMPILE_FLAGS "-m32" POSITION_INDEPENDENT_CODE ON)
