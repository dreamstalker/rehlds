cmake_minimum_required(VERSION 3.1)
project(proxy CXX)

option(DEBUG "Build with debug information." OFF)
option(USE_INTEL_COMPILER "Use the Intel compiler." OFF)
option(USE_CLANG_COMPILER "Use the Clang compiler." OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

	if (NOT DEBUG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ipo")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -ipo")
	endif()
else()
	# Produce code optimized for the most common IA32/AMD64/EM64T processors.
	# As new processors are deployed in the marketplace, the behavior of this option will change.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -msse3 -flto\
		-fpermissive\
		-Wno-unused-result -Wno-unknown-pragmas -Wno-unused-variable\
		-Wno-write-strings -Wno-strict-aliasing")

	if (USE_CLANG_COMPILER)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")
	endif()
endif()

set(PROJECT_SRC_DIR
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/../"
	"${PROJECT_SOURCE_DIR}/../Director/src"
)

set(PROJECT_BZIP2_DIR
	"${PROJECT_SOURCE_DIR}/../../../dep/bzip2/include"
)

set(PROJECT_PUBLIC_DIR
	"${PROJECT_SOURCE_DIR}/../.."
	"${PROJECT_SOURCE_DIR}/../../engine"
	"${PROJECT_SOURCE_DIR}/../../common"
	"${PROJECT_SOURCE_DIR}/../../pm_shared"
	"${PROJECT_SOURCE_DIR}/../../public"
	"${PROJECT_SOURCE_DIR}/../../public/rehlds"
)

set(PROXY_SRCS
	"src/Proxy.cpp"
	"src/Status.cpp"
	"src/Master.cpp"
	"src/ProxyClient.cpp"
	"src/DemoClient.cpp"
	"src/FakeClient.cpp"
	"src/public_amalgamation.cpp"
	"../Director/src/Director.cpp"
)

set(COMMON_SRCS
	"../../common/BaseSystemModule.cpp"
	"../../common/ObjectDictionary.cpp"
	"../../common/ObjectList.cpp"
	"../../common/TokenLine.cpp"
	"../../HLTV/common/BaseClient.cpp"
	"../../HLTV/common/BitBuffer.cpp"
	"../../HLTV/common/byteorder.cpp"
	"../../HLTV/common/common.cpp"
	"../../HLTV/common/DemoFile.cpp"
	"../../HLTV/common/DirectorCmd.cpp"
	"../../HLTV/common/InfoString.cpp"
	"../../HLTV/common/mathlib.cpp"
	"../../HLTV/common/md5.cpp"
	"../../HLTV/common/munge.cpp"
	"../../HLTV/common/NetAddress.cpp"
	"../../HLTV/common/NetChannel.cpp"
	"../../HLTV/common/random.cpp"
	"../../engine/mem.cpp"
)

include_directories(
	${PROJECT_SRC_DIR}
	${PROJECT_BZIP2_DIR}
	${PROJECT_PUBLIC_DIR}
)

link_directories(${PROJECT_SOURCE_DIR}/../../lib/linux32)

add_definitions(
	-DHLTV
	-DHLTV_FIXES
	-D_LINUX
	-DLINUX
	-D_GLIBCXX_USE_CXX11_ABI=0
	-D_stricmp=strcasecmp
	-D_strnicmp=strncasecmp
	-D_strdup=strdup
	-D_vsnprintf=vsnprintf
	-D_snprintf=snprintf
)

if (NOT TARGET bzip2)
	add_subdirectory(../../../dep/bzip2 lib)
endif()

add_library(proxy SHARED ${PROXY_SRCS} ${COMMON_SRCS})
target_link_libraries(proxy dl bzip2 steam_api)
set_target_properties(proxy PROPERTIES PREFIX "" COMPILE_FLAGS "-m32" LINK_FLAGS "-m32" POSITION_INDEPENDENT_CODE ON)
