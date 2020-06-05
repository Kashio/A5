cmake_minimum_required(VERSION 3.12.0)

project(A5
	VERSION 1.0.0
	DESCRIPTION "C++ custom memory allocators"
	LANGUAGES CXX)

option(A5_ENABLE_TESTING "Enable testing of the A5 library." ON)
option(A5_ENABLE_INSTALL "Enable installation of A5. (Projects embedding benchmark may want to turn this OFF.)" ON)

list(APPEND CMAKE_MODULE_PATH "${A5_SOURCE_DIR}/cmake")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(A5_INC_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
	set(A5_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
	set(A5_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(A5_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(A5_FRAMEWORK_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(A5_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/A5/cmake")
	set(A5_ADDITIONAL_FILES_INSTALL_DIR "CMAKE_INSTALL_LIBDIR/A5")
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	include(GNUInstallDirs)
	set(A5_INC_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
	set(A5_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
	set(A5_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(A5_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(A5_FRAMEWORK_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
	set(A5_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/A5/cmake")
	set(A5_ADDITIONAL_FILES_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/A5")
else()
	message(FATAL_ERROR "Could not set install folders for this platform!")
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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

add_subdirectory(src)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND A5_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(test)
	add_subdirectory(benchmark)
endif()
