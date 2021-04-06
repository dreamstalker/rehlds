#----------------------------------------
# 1. Preparing build:
#    rm -rf build
#    mkdir build && cd build
#
# 2. Select compiler and build it
# - Compile with Clang:
#    CC="clang" CXX="clang++" cmake ..
#    make
#
# - Compile with Intel C++ Compiler:
#    CC="icc" CXX="icpc" cmake ..
#    make
#
# - Compile with GCC Compiler:
#    cmake ..
#    make
#----------------------------------------

cmake_minimum_required(VERSION 3.1)
project(engine CXX)

option(DEBUG "Build with debug information." OFF)
option(USE_STATIC_LIBSTDC "Enables static linking libstdc++." OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_CXX_FLAGS "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti -fno-exceptions")

# Remove noxref code and data
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

if (DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O3 -ggdb")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g0 -O3 -fno-stack-protector")
endif()

# Check Intel C++ compiler
if ($ENV{CXX} MATCHES "icpc")
	# -fp-model=precise
	# ICC uses -fp-model fast=1 by default for more aggressive optimizations on floating-point calculations
	# https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/compiler-options/compiler-option-details/floating-point-options/fp-model-fp.html#fp-model-fp_GUID-99936BBA-1508-4E9F-AC09-FA98613CE2F5
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
		-fp-model=precise\
		-fasm-blocks\
		-Qoption,cpp,--treat_func_as_string_literal_cpp")

	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-intel -no-intel-extensions")

	if (NOT DEBUG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ipo")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -ipo")
	endif()
else()
	# Produce code optimized for the most common IA32/AMD64/EM64T processors.
	# As new processors are deployed in the marketplace, the behavior of this option will change.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
		-mtune=generic -msse3\
		-fpermissive -fno-sized-deallocation\
		-Wno-unknown-pragmas -Wno-invalid-offsetof\
		-Wno-unused-variable -Wno-unused-result -Wno-unused-function -Wno-delete-non-virtual-dtor\
		-Wno-write-strings -Wno-format\
		-Wno-sign-compare -Wno-strict-aliasing -Wno-ignored-attributes")

	# Check if not Clang compiler
	if (NOT $ENV{CXX} MATCHES "clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-plt -Wno-unused-but-set-variable")

		# GCC >= 8.3
		if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-truncation -Wno-format-truncation -Wno-class-memaccess")
		endif()
	endif()
endif()

# GCC >= 8.3
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcf-protection=none")
endif()

if (NOT DEBUG)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} \
		-Wl,-gc-sections -Wl,--version-script=\"${PROJECT_SOURCE_DIR}/../version_script.lds\"")
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

set(ENGINE_SRCS
	engine/sv_main.cpp
	engine/sv_user.cpp
	engine/sv_phys.cpp
	engine/sv_move.cpp
	engine/sv_pmove.cpp
	engine/sv_log.cpp
	engine/sv_remoteaccess.cpp
	engine/sv_steam3.cpp
	engine/sv_upld.cpp
	engine/sys_dll.cpp
	engine/sys_dll2.cpp
	engine/sys_engine.cpp
	engine/sys_linuxwind.cpp
	engine/SystemWrapper.cpp
	engine/host.cpp
	engine/host_cmd.cpp
	engine/net_chan.cpp
	engine/net_ws.cpp
	engine/pmove.cpp
	engine/pmovetst.cpp
	engine/pr_cmds.cpp
	engine/pr_edict.cpp
	engine/wad.cpp
	engine/model.cpp
	engine/world.cpp
	engine/zone.cpp
	engine/cmd.cpp
	engine/cmodel.cpp
	engine/com_custom.cpp
	engine/common.cpp
	engine/crc.cpp
	engine/cvar.cpp
	engine/decals.cpp
	engine/delta.cpp
	engine/delta_jit.cpp
	engine/ed_strpool.cpp
	engine/filesystem.cpp
	engine/filesystem_internal.cpp
	engine/hashpak.cpp
	engine/info.cpp
	engine/ipratelimit.cpp
	engine/l_studio.cpp
	engine/textures.cpp
	engine/tmessage.cpp
	engine/traceinit.cpp
	engine/unicode_strtools.cpp
	engine/buildnum.cpp
	engine/mathlib.cpp
	engine/mathlib_sse.cpp
	engine/md5.cpp
	engine/mem.cpp
	engine/module.cpp
	engine/r_studio.cpp
	engine/vid_null.cpp
	engine/cl_null.cpp
	engine/snd_null.cpp
	engine/sse_mathfun.cpp
	engine/public_amalgamation.cpp
	rehlds/flight_recorder.cpp
	rehlds/FlightRecorderImpl.cpp
	rehlds/hookchains_impl.cpp
	rehlds/main.cpp
	rehlds/platform.cpp
	rehlds/public_amalgamation.cpp
	rehlds/rehlds_api_impl.cpp
	rehlds/rehlds_interfaces_impl.cpp
	rehlds/rehlds_security.cpp
)

set(COMMON_SRCS
	"common/BaseSystemModule.cpp"
	"common/ObjectList.cpp"
	"common/TokenLine.cpp"
)

set(PUBLIC_SRCS
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
	-U_FORTIFY_SOURCE
	-D_stricmp=strcasecmp
	-D_strnicmp=strncasecmp
	-D_strdup=strdup
	-D_unlink=unlink
	-D_vsnprintf=vsnprintf
	-D_vsnwprintf=vswprintf
)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} \
	-Wl,-rpath,'$ORIGIN/.' \
	-L${PROJECT_SOURCE_DIR}/lib/linux32")

if (NOT TARGET bzip2)
	add_subdirectory(../dep/bzip2 lib)
endif()

if (NOT TARGET appversion)
	add_custom_target(appversion DEPENDS COMMAND "${PROJECT_SOURCE_DIR}/version/appversion.sh" "${PROJECT_SOURCE_DIR}/..")
endif()

add_library(engine SHARED ${appversion.sh} ${ENGINE_SRCS} ${COMMON_SRCS} ${PUBLIC_SRCS})
add_dependencies(engine appversion)

set_target_properties(engine PROPERTIES
	LIBRARY_OUTPUT_NAME engine_i486
	PREFIX ""
	COMPILE_FLAGS "-m32"
	LINK_FLAGS "-m32"
	POSITION_INDEPENDENT_CODE OFF
)

target_link_libraries(engine dl rt m aelf32 bzip2 steam_api)
