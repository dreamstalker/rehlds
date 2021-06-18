cmake_minimum_required(VERSION 3.1)
project(cppunitlite CXX)

add_library(cppunitlite STATIC)

target_sources(cppunitlite PRIVATE
	src/Test.cpp
	src/TestResult.cpp
	src/TestRegistry.cpp
	src/Assertions.cpp
	src/MainAdapter.cpp
)

target_include_directories(cppunitlite PRIVATE
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/include"
)

target_compile_definitions(cppunitlite PRIVATE
	_GLIBCXX_USE_CXX11_ABI=0
)

target_compile_options(cppunitlite PRIVATE
	-m32
)
