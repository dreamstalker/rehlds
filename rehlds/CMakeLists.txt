cmake_minimum_required(VERSION 3.1)
project(engine CXX)

option(DEBUG "Build with debug information." OFF)
option(USE_INTEL_COMPILER "Use the Intel compiler." OFF)
option(USE_CLANG_COMPILER "Use the Clang compiler." OFF)
option(USE_STATIC_LIBSTDC "Enables static linking libstdc++." OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (USE_INTEL_COMPILER)
	set(CMAKE_C_COMPILER	"/opt/intel/bin/icc")
	set(CMAKE_CXX_COMPILER	"/opt/intel/bin/icpc")
elseif (USE_CLANG_COMPILER)
	set(CMAKE_C_COMPILER	"/usr/bin/clang")
	set(CMAKE_CXX_COMPILER	"/usr/bin/clang++")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti -fno-exceptions")

# Remove noxref code and data
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -msse3\
		-fpermissive\
		-Wno-unknown-pragmas -Wno-invalid-offsetof\
		-Wno-unused-variable -Wno-unused-result -Wno-unused-function -Wno-delete-non-virtual-dtor\
		-Wno-write-strings -Wno-format\
		-Wno-sign-compare -Wno-strict-aliasing -Wno-ignored-attributes")

	if (NOT USE_CLANG_COMPILER)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-truncation -Wno-format-truncation -Wno-unused-but-set-variable -Wno-class-memaccess")
	endif()
endif()

if (NOT DEBUG AND USE_STATIC_LIBSTDC)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-gc-sections -Wl,--version-script=\"${PROJECT_SOURCE_DIR}/../version_script.lds\"")
endif()

if (USE_STATIC_LIBSTDC)
	add_definitions(-DBUILD_STATIC_LIBSTDC)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

set(PROJECT_SRC_DIR
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/engine"
	"${PROJECT_SOURCE_DIR}/common"
	"${PROJECT_SOURCE_DIR}/pm_shared"
	"${PROJECT_SOURCE_DIR}/rehlds"
	"${PROJECT_SOURCE_DIR}/testsuite"
)

set(PROJECT_BZIP2_DIR
	"${PROJECT_SOURCE_DIR}/../dep/bzip2/include"
)

set(PROJECT_PUBLIC_DIR
	"${PROJECT_SOURCE_DIR}/public"
	"${PROJECT_SOURCE_DIR}/public/rehlds"
)

file(GLOB ENGINE_SRCS
	"engine/*.cpp"
	"rehlds/*.cpp"
	"version/*.cpp"
)

list(REMOVE_ITEM ENGINE_SRCS EXCLUDE
	"${PROJECT_SOURCE_DIR}/rehlds/precompiled.cpp"
	"${PROJECT_SOURCE_DIR}/rehlds/RehldsRuntimeConfig.cpp"
	"${PROJECT_SOURCE_DIR}/rehlds/structSizeCheck.cpp"
)

file(GLOB COMMON_SRCS
	"common/BaseSystemModule.cpp"
	"common/ObjectList.cpp"
	"common/TokenLine.cpp"
)

file(GLOB PUBLIC_SRCS
	"public/tier0/dbg.cpp"
	"public/registry.cpp"
	"public/steamid.cpp"
	"public/utlbuffer.cpp"
)

include_directories(
	${PROJECT_SRC_DIR}
	${PROJECT_BZIP2_DIR}
	${PROJECT_PUBLIC_DIR}
)

link_directories(${PROJECT_SOURCE_DIR}/lib/linux32)

add_definitions(
	-DSWDS
	-DREHLDS_JIT
	-DREHLDS_SSE
	-DREHLDS_FIXES
	-DREHLDS_CHECKS
	-DREHLDS_API
	-DREHLDS_SELF
	-DREHLDS_OPT_PEDANTIC
	-DHAVE_OPT_STRTOOLS
	-DUSE_BREAKPAD_HANDLER
	-D_LINUX
	-DLINUX
	-D_GLIBCXX_USE_CXX11_ABI=0
	-D_stricmp=strcasecmp
	-D_strnicmp=strncasecmp
	-D_strdup=strdup
	-D_unlink=unlink
	-D_vsnprintf=vsnprintf
	-D_vsnwprintf=vswprintf
)

if (NOT TARGET bzip2)
	add_subdirectory(../dep/bzip2 lib)
endif()

if (NOT TARGET appversion)
	add_custom_target(appversion DEPENDS COMMAND "${PROJECT_SOURCE_DIR}/version/appversion.sh" "${PROJECT_SOURCE_DIR}")
endif()

add_library(engine SHARED ${appversion.sh} ${ENGINE_SRCS} ${COMMON_SRCS} ${PUBLIC_SRCS})
set_property(TARGET engine PROPERTY LIBRARY_OUTPUT_NAME engine_i486)
set_target_properties(engine PROPERTIES PREFIX "" COMPILE_FLAGS "-m32" LINK_FLAGS "-m32" POSITION_INDEPENDENT_CODE ON)
target_link_libraries(engine dl rt m aelf32 bzip2 steam_api)
add_dependencies(engine appversion)
