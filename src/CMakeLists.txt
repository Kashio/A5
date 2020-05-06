file(GLOB_RECURSE SOURCES "*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../include/A5/*.h")

add_library(A5 STATIC ${SOURCES})

target_include_directories(A5 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")