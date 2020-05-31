add_executable(sandbox main.cpp)

include(FetchContent)
FetchContent_Declare(googlebenchmark
                     GIT_REPOSITORY https://github.com/google/benchmark
					 GIT_TAG master
        )
FetchContent_MakeAvailable(googlebenchmark)

target_link_libraries(sandbox PRIVATE A5 benchmark::benchmark_main)
