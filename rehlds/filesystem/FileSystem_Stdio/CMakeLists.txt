cmake_minimum_required(VERSION 3.1)
project(filesystem_stdio CXX)

option(DEBUG "Build with debug information." OFF)
option(USE_INTEL_COMPILER "Use the Intel compiler." OFF)
option(USE_CLANG_COMPILER "Use the Clang compiler." OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WRAP_FUNCS_LIST
	"scandir" "opendir" "freopen" "fopen" "fopen64" "open" "open64" "creat"
	"access" "stat" "lstat" "__xstat" "__lxstat" "__xstat64" "__lxstat64"
	"chmod" "chown" "lchown" "unlink" "symlink" "link" "mknod" "mount"
	"mkfifo" "rename" "utime" "utimes" "mkdir" "rmdir"
)

if (USE_INTEL_COMPILER)
	set(CMAKE_C_COMPILER	"/opt/intel/bin/icc")
	set(CMAKE_CXX_COMPILER	"/opt/intel/bin/icpc")
elseif (USE_CLANG_COMPILER)
	set(CMAKE_C_COMPILER	"/usr/bin/clang")
	set(CMAKE_CXX_COMPILER	"/usr/bin/clang++")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions")

if (DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O3 -ggdb")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g0 -O3")
endif()

if (USE_INTEL_COMPILER)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qoption,cpp,--treat_func_as_string_literal_cpp")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-intel -no-intel-extensions")
else()
	# Produce code optimized for the most common IA32/AMD64/EM64T processors.
	# As new processors are deployed in the marketplace, the behavior of this option will change.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -msse3\
		-fpermissive\
		-Wno-unknown-pragmas -Wno-unused-result -Wno-unused-variable -Wno-unused-function\
		-Wno-write-strings -Wno-sign-compare")

	if (NOT USE_CLANG_COMPILER)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-truncation -Wno-class-memaccess")
	endif()
endif()

foreach(f ${WRAP_FUNCS_LIST})
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-wrap,${f}")
endforeach()

set(PROJECT_SRC_DIR
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/../.."
)

set(PROJECT_PUBLIC_DIR
	"${PROJECT_SOURCE_DIR}/../../common"
	"${PROJECT_SOURCE_DIR}/../../public"
	"${PROJECT_SOURCE_DIR}/../../public/rehlds"
)

set(FILESYSTEM_STDIO_SRCS
	"src/BaseFileSystem.cpp"
	"src/filesystem_helpers.cpp"
	"src/FileSystem_Stdio.cpp"
	"src/linux_support.cpp"
	"src/pathmatch.cpp"
	"src/public_amalgamation.cpp"
)

include_directories(
	${PROJECT_SRC_DIR}
	${PROJECT_PUBLIC_DIR}
)

add_definitions(
	-D_LINUX
	-DLINUX
	-D_GLIBCXX_USE_CXX11_ABI=0
	-D_strdup=strdup
	-D_stricmp=strcasecmp
	-D_strnicmp=strncasecmp
	-D_vsnprintf=vsnprintf
	-D_snprintf=snprintf
	-D_unlink=unlink
)

add_library(filesystem_stdio SHARED ${FILESYSTEM_STDIO_SRCS})
set_target_properties(filesystem_stdio PROPERTIES PREFIX "" COMPILE_FLAGS "-m32" LINK_FLAGS "-m32" POSITION_INDEPENDENT_CODE ON)
target_link_libraries(filesystem_stdio dl)
