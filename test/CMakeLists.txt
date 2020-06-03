add_executable(testlib AllocateTest.cpp Utils.cpp)

FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/google/googletest
					 GIT_TAG master
        )
FetchContent_MakeAvailable(googletest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

target_link_libraries(testlib PRIVATE A5 gtest_main)

add_test(testlib testlib)