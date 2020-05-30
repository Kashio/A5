add_library(A5
			"${CMAKE_SOURCE_DIR}/include/A5/Allocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/LinearAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/StackAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/PoolAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/FreeListAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/FreeTreeAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/BuddyAllocator.h"
			"${CMAKE_SOURCE_DIR}/include/A5/RBTree.h"
			"${CMAKE_CURRENT_SOURCE_DIR}/LinearAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/StackAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/PoolAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/FreeListAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/FreeTreeAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/BuddyAllocator.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/RBTree.cpp"
			)

target_include_directories(A5 PUBLIC "${CMAKE_SOURCE_DIR}/include")

target_compile_features(A5 PUBLIC cxx_std_11)