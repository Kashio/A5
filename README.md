# A5 C++ Library

The A5 library is a collection of 5 custom memory allocators that can be used instead of the default malloc/new functions
inorder to manage a large heap of memory ourselves and save the memory request from the OS while also keeping fragmentation to minmimum
and data locality to maximum.

## Installation
The library requires C++11.

You can use it as a git submodule inside your project:
```bash
mkdir extern
cd extern
git submodule add https://github.com/Kashio/A5.git
```
then add it as a subdirectory to your main `CMakeLists.txt`:
```cmake
add_subdirectory(extern/A5)
```
and link againts it:
```cmake
add_executable(app main.cpp)

target_link_libraries(app PRIVATE A5)
```
and then just build:
```bash
mkdir build
cd build
cmake ..
```

You can also just use `FetchContent` without git submodules:
```cmake
add_executable(app main.cpp)

FetchContent_Declare(A5
                     GIT_REPOSITORY https://github.com/Kashio/A5.git
					 GIT_TAG master
        )
FetchContent_MakeAvailable(A5)

target_link_libraries(app PRIVATE A5)
```

To install the library in the system in path specified by `${CMAKE_INSTALL_PREFIX}`:
```bash
git clone https://github.com/Kashio/A5.git
cd A5
mkdir build
cd build
cmake ..
cmake --build . --target install
```
and then you can just use `find_package(my_library ${A5_version_wanted} REQUIRED)`.

To change between Debug/Release when building call cmake as follows
on single-configurations generators:
```bash
cmake .. -DCMAKE_BUILD_TYPE="buildtype" 
```
on multi-configurations generators:
```bash
cmake --build . --config buildtype
```

You can disable install and testing by setting these variables which are `ON` by default (no need to set these for projects that include them):
```
A5_ENABLE_INSTALL
A5_ENABLE_TESTING
```
