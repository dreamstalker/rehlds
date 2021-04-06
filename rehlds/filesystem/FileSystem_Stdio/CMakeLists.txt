cmake_minimum_required(VERSION 3.1)
project(filesystem_stdio CXX)

option(DEBUG "Build with debug information." OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_CXX_FLAGS "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions")

if (DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O3 -ggdb")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g0 -O3 -fno-stack-protector")
endif()

# Check Intel C++ compiler
if ($ENV{CXX} MATCHES "icpc")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qoption,cpp,--treat_func_as_string_literal_cpp")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-intel -no-intel-extensions")
else()
	# Produce code optimized for the most common IA32/AMD64/EM64T processors.
	# As new processors are deployed in the marketplace, the behavior of this option will change.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
		-mtune=generic -msse3\
		-fpermissive -fno-sized-deallocation\
		-Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-result -Wno-unused-function\
		-Wno-write-strings -Wno-sign-compare")

	# Check if not Clang compiler AND GCC >= 8.3
	if (NOT $ENV{CXX} MATCHES "clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-truncation -Wno-class-memaccess")
	endif()
endif()

# NOTE: Don't use IPO or LTO with '--wrap' may make this feature ineffective
set(WRAP_FUNCS_LIST
	"scandir" "opendir" "freopen" "fopen" "fopen64" "open" "open64" "creat"
	"access" "stat" "lstat" "__xstat" "__lxstat" "__xstat64" "__lxstat64"
	"chmod" "chown" "lchown" "unlink" "symlink" "link" "mknod" "mount"
	"mkfifo" "rename" "utime" "utimes" "mkdir" "rmdir"
)

foreach(f ${WRAP_FUNCS_LIST})
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-wrap,${f}")
endforeach()

# GCC >= 8.3
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcf-protection=none")
endif()

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
	-U_FORTIFY_SOURCE
	-D_strdup=strdup
	-D_stricmp=strcasecmp
	-D_strnicmp=strncasecmp
	-D_vsnprintf=vsnprintf
	-D_snprintf=snprintf
	-D_unlink=unlink
)

add_library(filesystem_stdio SHARED ${FILESYSTEM_STDIO_SRCS})

set_target_properties(filesystem_stdio PROPERTIES
	LIBRARY_OUTPUT_NAME filesystem_stdio
	PREFIX ""
	COMPILE_FLAGS "-m32"
	LINK_FLAGS "-m32"
	POSITION_INDEPENDENT_CODE OFF
)

target_link_libraries(filesystem_stdio dl)
