add_executable(benchmarks main.cpp Common.h Common.cpp Allocations.cpp)

FetchContent_Declare(googlebenchmark
                     GIT_REPOSITORY https://github.com/google/benchmark
					 GIT_TAG master
        )
FetchContent_MakeAvailable(googlebenchmark)

target_link_libraries(benchmarks PRIVATE A5 benchmark::benchmark)
