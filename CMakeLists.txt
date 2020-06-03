cmake_minimum_required(VERSION 3.12.0)
project(A5
	VERSION 1.0.0
	DESCRIPTION "C++ custom memory allocators"
	LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

	set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    include(CTest)

    find_package(Doxygen)
    if(Doxygen_FOUND)
		add_subdirectory(docs)
    else()
		message(STATUS "Doxygen not found, not building docs")
    endif()
endif()


include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
   include(add_FetchContent_MakeAvailable)
endif()

set(BENCHMARK_ENABLE_TESTING OFF)

add_subdirectory(src)
add_subdirectory(benchmark)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_TESTING)
    add_subdirectory(test)
endif()
