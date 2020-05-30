cmake_minimum_required(VERSION 3.12.0)
project(A5
	VERSION 1.0.0
	DESCRIPTION "C++ custom memory allocators"
	LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
   include(add_FetchContent_MakeAvailable)
endif()


add_subdirectory(src)
add_subdirectory(sandbox)