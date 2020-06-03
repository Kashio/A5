add_executable(benchmarks Allocations.cpp Utils.cpp)

FetchContent_Declare(googlebenchmark
                     GIT_REPOSITORY https://github.com/google/benchmark
					 GIT_TAG master
        )
FetchContent_MakeAvailable(googlebenchmark)

target_link_libraries(benchmarks PRIVATE A5 benchmark::benchmark_main)
