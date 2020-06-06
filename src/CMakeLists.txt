set(headers
	"${A5_SOURCE_DIR}/include/A5/Allocator.h"
	"${A5_SOURCE_DIR}/include/A5/CAllocator.h"
	"${A5_SOURCE_DIR}/include/A5/LinearAllocator.h"
	"${A5_SOURCE_DIR}/include/A5/StackAllocator.h"
	"${A5_SOURCE_DIR}/include/A5/PoolAllocator.h"
	"${A5_SOURCE_DIR}/include/A5/FreeListAllocator.h"
	"${A5_SOURCE_DIR}/include/A5/FreeTreeAllocator.h"
	"${A5_SOURCE_DIR}/include/A5/BuddyAllocator.h"
	"${A5_SOURCE_DIR}/include/A5/LinkedList.h"
	"${A5_SOURCE_DIR}/include/A5/RBTree.h")

add_library(A5
			${headers}
			"CAllocator.cpp"
			"LinearAllocator.cpp"
			"StackAllocator.cpp"
			"PoolAllocator.cpp"
			"FreeListAllocator.cpp"
			"FreeTreeAllocator.cpp"
			"BuddyAllocator.cpp"
			"LinkedList.cpp"
			"RBTree.cpp"
			)

target_include_directories(A5 PUBLIC
                       $<BUILD_INTERFACE:${A5_SOURCE_DIR}/include>
                       $<INSTALL_INTERFACE:${A5_INC_INSTALL_DIR}>
					   )

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${A5_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
	"${project_config}"
	INSTALL_DESTINATION ${A5_CMAKE_CONFIG_INSTALL_DIR})
write_basic_package_version_file("${version_config}" COMPATIBILITY SameMajorVersion)

if (A5_ENABLE_INSTALL)
	install(TARGETS A5
			EXPORT ${targets_export_name}
			ARCHIVE DESTINATION ${A5_ARCHIVE_INSTALL_DIR}
			LIBRARY DESTINATION ${A5_LIBRARY_INSTALL_DIR}
			RUNTIME DESTINATION ${A5_RUNTIME_INSTALL_DIR}
			FRAMEWORK DESTINATION ${A5_FRAMEWORK_INSTALL_DIR})

	install(
		DIRECTORY "${A5_SOURCE_DIR}/include/A5"
		DESTINATION ${A5_INC_INSTALL_DIR}
		FILES_MATCHING PATTERN "*.*h")

	install(
      FILES "${project_config}" "${version_config}"
      DESTINATION "${A5_CMAKE_CONFIG_INSTALL_DIR}")

	install(
		EXPORT ${targets_export_name}
		DESTINATION ${A5_CMAKE_CONFIG_INSTALL_DIR})
endif()
