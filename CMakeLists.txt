cmake_minimum_required(VERSION 3.1)
project(rehlds CXX)

if (WIN32)
	message(FATAL_ERROR "CMakeLists.txt Windows platform isn't supported yet. Use msvc/ReHLDS.sln instead it!")
endif()

add_custom_target(appversion DEPENDS
	COMMAND "${PROJECT_SOURCE_DIR}/rehlds/version/appversion.sh" "${PROJECT_SOURCE_DIR}"
)

add_subdirectory(rehlds)

if (NOT CMAKE_BUILD_TYPE MATCHES Unittests)
	add_subdirectory(rehlds/dedicated)
	add_subdirectory(rehlds/filesystem)
	add_subdirectory(rehlds/HLTV)
endif()
