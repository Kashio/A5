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
and then you can just use `find_package(A5 ${A5_version_wanted} REQUIRED)`.

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
## Allocators
### Linear Allocator
Fastest allocation, calculate address to be multiple of alignment and return a pointer to that address.
Low on overhead memory since it's not need to keep any kind of book keeping structure becase it only allows deallocation of the whole buffer and not of specific addreeses. There's minimum internal fragmentation that is caused by the alignment of the returned addresses.
It is best to use this allocator when you only need to deallocate everything at once.

### Stack Allocator
Fastest allocation after Linear Allocator, calculate address to be multiple of alignment and return a pointer to that address.
Low on overhead memory, but uses a header to keep the padding required for the current allocated address to be aligned. This header is only 1 byte in size so it can be aligned to any address and be adjacent to the allocated block. This header is used for deallocating addreeses in a stack like order, so you can only deallocate in reverse order of allocation.
There's minimum internal fragmentation that is caused by the alignment of the returned address and the header for saving it.
It is best to use this allocator when you can deallocate in a last in first out order.

### Pool Allocator
Pool allocator is a fixed size allocator, meaning you can only allocate chunks of predefined size.
The allocator keeps an explicit linked list of free chunks, and so when a request to allocate is called, the first available chunk is returned and the list updates accordingly.
There's no memory overhead to this allocator because the free chunks list is stored inside the memory area allocated by the allocator itself to save memory, and each chunk is replaced completetly by the data of the payload. To make sure your addresses are alligned correctly, specify a chunk size of the next multiple of your required structure alignment that is bigger than your structure size.
For example `sizeof(S) == 42` and `alignof(S) == 8` then pass **48** as the chunk size.
This can be calculated in a generalized form as follows:
```cpp
sizeof(S) % alignof(S) == 0 ? sizeof(S) : sizeof(S) + alignof(S) - sizeof(S) % alignof(S);
```
There will be fragmentation only if `sizeof(S)` is not a multiple of `alignof(S)` and a different chunk size is needed to be calculated inorder for the data to be properly aligned.
It is best to use this allocator when you need to allocate data of a specific size and you need to deallocate an address at anytime regardless of the other allocations.

### Free List Allocator
Free list allocator, is the same as pool allocator, but it allows for any size of data to be allocated.
The alloctor keeps an explicit linked list of free nodes, and so when a request to allocate is called, a node with sufficient size is returned and the list updates accordingly.
There's a memory overhead to this allocator, each allocation is also storing a header that saves the size of the allocated block, but the explicit linked list of free nodes is stored inside the memory area allocated by the alloctor itself to save some memory.
There's internal fragmentation depending on the size of the allocation, since header is defined with alignment `alignas(std::max_align_t)` so the address of the allocated block will always be aligned regardless of size, but to make sure the next header in the list is aligned we need to calculate padding to the next address that is aligned to `std::max_align_t`.
The allocator can search for the first fit free node it finds, or it can also search for the be fit node it finds.
Usually best fit is better, although it searches the entire free list for a node, that usually means that larger nodes are not spent on small allocations and thus making future allocations potentially faster than first fit. Best fit also reduce internal fragmentation needed for padding.
When a node is found it can be potentially splitted to another node if there's anough space.
Also when an address is deallocated it can be merged with the 2 adjacent nodes to it, if they're free. 
It is best to use this allocator when you need to allocate data of different sizes and you need to deallocate an address at anytime regardless of other allocations.

### Free Tree Allocator
Free tree allocator, is the same as free list allocator, but instead of the free nodes being stored as an explicit linked list, they're being stored inside a red black tree that is also stored inside the memory area allocated by the alloctor itself to save some memory.
This red black tree structure allows us to do allocation and deallocation operations in **O(LogN)** instead of **O(N)** like in the free list alloctor.
There's a memory overhead to this alloctor, each allocation is also storing a header that saves the size of the allocated block as-well as the size of the previous block (if allocated). This previous block size helps us to do merging in **O(1)**.
By the nature of the red black tree structure this allocator support best fit search of free node.
The internal fragmentation is the same as the free list allocator.
This is generally a better version of the free list allocator so in most cases it will be better to use it depending on the pattern of your allocations and deallocations.

### Buddy Allocator
Buddy allocator is an allocator that saves free blocks of sizes of power of 2 up to the nearest power of 2 to the allocator size, that is greater or equal to the size.
For example a buddy allocator with size of **120** bytes will have blocks of sizes `8, [16, 32], 64, 128`.
The reason **16** and **32** are inside square brackets is that it depends on a **header** struct size which stores how many bytes the current block is storing,
So for x64 **header** size will be 16 bytes and for x32 the **header** size will be 8 bytes, and so it is a requirement,
that the blocks must be atleast **header** size bytes to **just** to contain the **header** it self, so we are basically storing the power of 2 that is one
greater than the power of 2 of the **header** size.
When a request to allocate is called, the allocator will find the best fitted size block for the allocation, and if non are free,
then it will split higher order blocks to smaller order blocks, until the block size of the desired order is created.
Similarly, when deallocating, blocks that were splitted before by the allocation process, will be merged together to higher order block if both are free.
There's internal fragmentation by the nature of this allocator, if a requested allocation size is not multiple of power of 2 then some of the memory of the blocks will go to waste,
and also some of the memory is occupied by the **header** to know how much memory is occupied by the current memory block.
It is best to use this allocator when your data is size of power of 2 to keep internal fragmentation lower, but generally it is more slower than other general uses allocators.

## Benchmarks
Note that in all benchmarks the size of allocated block given to the Pool allocator is **4096**.
The following chart shows the sequential allocations of random sized blocks up to **100,000** blocks
where all allocators gets the same random sizes:
![figure_2](assets/Figure_2.png?raw=true)
You might be wondering how the free list allocator outperform the tree list allocator.
Well since all this benchmark did is to allocate and no deallocate at all, it was the best case **O(1)** for finding a free node in the linked list
while in the tree list allocator you pay extra penalty for each allocation where the tree update and it's more costly than the linked list update.
Where the tree list allocator really shines and outperform the free list allocator is where there're starting to develop gaps between the nodes and
the allocation is not just being sequentially which might be more close to what real applications allocation and deallocation patterns might look like.
The following chart show the performance of each allocator doing sequential allocations of random sized blocks up to **100,000** blocks like before,
while each block has **25%** to be later deallocated. After the deallocation phase, we go over and try to allocate another batch of **100,000** blocks
while this time there're holes inside the memory of each allocator:
![figure_3](assets/Figure_3.png?raw=true)

While you might be biased by these charts on which allocator to pick,
it is always best to pick the most strict allocator while also comforting to your data requirements because they're often the fastest one,
and be sure to benchmark your application with different allocators since as can be seen different allocation & deallocation patterns might lead to different results.

## Future work
* Make use of red black tree to store buddies inside buddy allocator where the value is the block address
to make deallocation **O(LogN)** instead of of **O(N)** where **N** is the number of free blocks in each bucket of power of 2.
* Right better tests. Right now the provided tests are just sanity ones to check the allocators are not doing some weird stuff
and overwriting values of other memory blocks.
* Research on how to layer a slab allocator on top of buddy allocator to reduce internal fragmentation.
* Rewrite this whole readme to be better..