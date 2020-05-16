add_library(A5 STATIC
			"${CMAKE_SOURCE_DIR}/include/A5/Allocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/LinearAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/StackAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/PoolAllocator.h"
			"${CMAKE_CURRENT_SOURCE_DIR}/LinearAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/StackAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/PoolAllocator.cpp"
			)

target_include_directories(A5 PUBLIC "${CMAKE_SOURCE_DIR}/include")