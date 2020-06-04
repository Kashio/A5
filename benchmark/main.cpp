#include <benchmark/benchmark.h>
#include "Common.h"
#include "Allocations.cpp"

static void SetUp()
{
	s_Sizes = { 1, 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 4096, 8192 };
	s_SizesAgg = 1 + 2 + 4 + 8 + 16 + 32 + 64 + 256 + 512 + 1024 + 4096 + 8192;
	s_RandomSizes.reserve(s_NumOfRandAllocations);
	for (std::size_t i = 0; i < s_RandomSizes.capacity(); ++i)
	{
		std::size_t size = (std::size_t)Utils::SelectRandomly(1, (int)s_MaxBlockSize);
		s_RandomSizes.push_back(size);
		if (Utils::SelectRandomly(1, 4) == 1)
			s_DeallocationIndices.push_back(i);
		s_RandomSizesAgg += size;
	}
}

BENCHMARK(Allocate_CAllocator);
BENCHMARK(Allocate_LinearAllocator);
BENCHMARK(Allocate_StackAllocator);
BENCHMARK(Allocate_PoolAllocator);
BENCHMARK(Allocate_FreeListAllocator);
BENCHMARK(Allocate_FreeTreeAllocator);
BENCHMARK(Allocate_BuddyAllocator);

BENCHMARK(RandomAllocate_CAllocator);
BENCHMARK(RandomAllocate_LinearAllocator);
BENCHMARK(RandomAllocate_StackAllocator);
BENCHMARK(RandomAllocate_FreeListAllocator);
BENCHMARK(RandomAllocate_FreeTreeAllocator);
BENCHMARK(RandomAllocate_BuddyAllocator);

BENCHMARK(RandomAllocateAndFree_CAllocator);
BENCHMARK(RandomAllocateAndFree_LinearAllocator);
BENCHMARK(RandomAllocateAndFree_StackAllocator);
BENCHMARK(FixedAllocateAndRandomFree_PoolAllocator);
BENCHMARK(RandomAllocateAndFree_FreeListAllocator);
BENCHMARK(RandomAllocateAndFree_FreeTreeAllocator);
BENCHMARK(RandomAllocateAndFree_BuddyAllocator);

int main(int argc, char** argv)
{
	SetUp();
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
}