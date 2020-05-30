add_executable(sandbox main.cpp)

target_link_libraries(sandbox PRIVATE A5)

include(FetchContent)
FetchContent_Declare(googlebenchmark
                     GIT_REPOSITORY https://github.com/google/benchmark
        )
FetchContent_MakeAvailable(googlebenchmark)

target_link_libraries(bench benchmark::benchmark)