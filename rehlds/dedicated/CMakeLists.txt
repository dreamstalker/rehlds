cmake_minimum_required(VERSION 3.1)
project(hlds CXX)

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

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")

if (USE_INTEL_COMPILER)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qoption,cpp,--treat_func_as_string_literal_cpp")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-intel -no-intel-extensions")

	if (NOT DEBUG)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ipo")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -ipo")
	endif()
else()
	# Produce code optimized for the most common IA32/AMD64/EM64T processors.
	# As new processors are deployed in the marketplace, the behavior of this option will change.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -msse3 -flto\
		-fpermissive\
		-Wno-unused-result")
endif()

set(PROJECT_SRC_DIR
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/../"
)

set(PROJECT_PUBLIC_DIR
	"${PROJECT_SOURCE_DIR}/../engine"
	"${PROJECT_SOURCE_DIR}/../common"
	"${PROJECT_SOURCE_DIR}/../public"
	"${PROJECT_SOURCE_DIR}/../public/rehlds"
)

set(DEDICATED_SRCS
	"src/dbg.cpp"
	"src/dedicated_exports.cpp"
	"src/public_amalgamation.cpp"
	"src/sys_ded.cpp"
	"src/sys_linux.cpp"
	"src/vgui/vguihelpers.cpp"
)

set(COMMON_SRCS
	"../common/textconsole.cpp"
	"../common/TextConsoleUnix.cpp"
	"../common/SteamAppStartUp.cpp"
	"../common/ObjectList.cpp"
	"../common/commandline.cpp"
	"../common/minidump.cpp"
	"../engine/mem.cpp"
)

include_directories(
	${PROJECT_SRC_DIR}
	${PROJECT_PUBLIC_DIR}
)

add_definitions(
	-DLAUNCHER_FIXES
	-D_CONSOLE
	-D_LINUX
	-DLINUX
	-D_GLIBCXX_USE_CXX11_ABI=0
	-D_stricmp=strcasecmp
	-D_strnicmp=strncasecmp
	-D_strdup=strdup
	-D_vsnprintf=vsnprintf
)

add_executable(hlds ${DEDICATED_SRCS} ${COMMON_SRCS})
target_link_libraries(hlds dl)
set_target_properties(hlds PROPERTIES OUTPUT_NAME hlds_linux PREFIX "" COMPILE_FLAGS "-m32" LINK_FLAGS "-m32" POSITION_INDEPENDENT_CODE ON)
