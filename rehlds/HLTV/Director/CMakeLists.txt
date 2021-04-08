cmake_minimum_required(VERSION 3.1)
project(director CXX)

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

	if (NOT DEBUG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ipo")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -ipo")
	endif()
else()
	# Produce code optimized for the most common IA32/AMD64/EM64T processors.
	# As new processors are deployed in the marketplace, the behavior of this option will change.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
		-mtune=generic -msse3 -flto\
		-fpermissive -fno-sized-deallocation\
		-Wno-unused-result -Wno-unknown-pragmas\
		-Wno-write-strings -Wno-strict-aliasing")
endif()

# GCC >= 8.3
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcf-protection=none")
endif()

set(PROJECT_SRC_DIR
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/../"
)

set(PROJECT_PUBLIC_DIR
	"${PROJECT_SOURCE_DIR}/../.."
	"${PROJECT_SOURCE_DIR}/../../engine"
	"${PROJECT_SOURCE_DIR}/../../common"
	"${PROJECT_SOURCE_DIR}/../../pm_shared"
	"${PROJECT_SOURCE_DIR}/../../public"
	"${PROJECT_SOURCE_DIR}/../../public/rehlds"
)

set(DIRECTOR_SRCS
	"src/Director.cpp"
	"src/public_amalgamation.cpp"
)

set(COMMON_SRCS
	"../../common/BaseSystemModule.cpp"
	"../../common/ObjectDictionary.cpp"
	"../../common/ObjectList.cpp"
	"../../common/TokenLine.cpp"
	"../../HLTV/common/BitBuffer.cpp"
	"../../HLTV/common/byteorder.cpp"
	"../../HLTV/common/common.cpp"
	"../../HLTV/common/DirectorCmd.cpp"
	"../../HLTV/common/mathlib.cpp"
	"../../HLTV/common/random.cpp"
	"../../engine/mem.cpp"
)

include_directories(
	${PROJECT_SRC_DIR}
	${PROJECT_PUBLIC_DIR}
)

add_definitions(
	-DHLTV
	-DHLTV_FIXES
	-DDIRECTOR_MODULE
	-D_LINUX
	-DLINUX
	-D_GLIBCXX_USE_CXX11_ABI=0
	-U_FORTIFY_SOURCE
	-D_stricmp=strcasecmp
	-D_strnicmp=strncasecmp
	-D_strdup=strdup
	-D_vsnprintf=vsnprintf
	-D_snprintf=snprintf
)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} \
	-Wl,-rpath,'$ORIGIN/.' \
	-L${PROJECT_SOURCE_DIR}/../../lib/linux32")

add_library(director SHARED ${DIRECTOR_SRCS} ${COMMON_SRCS})
target_link_libraries(director dl m)
set_target_properties(director PROPERTIES
	LIBRARY_OUTPUT_NAME director
	PREFIX ""
	COMPILE_FLAGS "-m32"
	LINK_FLAGS "-m32"
	POSITION_INDEPENDENT_CODE OFF
)
