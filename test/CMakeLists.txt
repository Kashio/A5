add_executable(testlib AllocateTest.cpp Utils.cpp)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/google/googletest
					 GIT_TAG master
        )
FetchContent_MakeAvailable(googletest)

target_link_libraries(testlib PRIVATE A5 gtest_main)

add_test(testlib testlib)