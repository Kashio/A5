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

BENCHMARK(Allocate_CAllocator)->DenseRange(0, s_Sizes.size(), 1)->Unit(benchmark::kMillisecond);
BENCHMARK(Allocate_LinearAllocator)->DenseRange(0, s_Sizes.size(), 1)->Unit(benchmark::kMillisecond);
BENCHMARK(Allocate_StackAllocator)->DenseRange(0, s_Sizes.size(), 1)->Unit(benchmark::kMillisecond);
BENCHMARK(Allocate_PoolAllocator)->DenseRange(0, s_Sizes.size(), 1)->Unit(benchmark::kMillisecond);
BENCHMARK(Allocate_FreeListAllocator)->DenseRange(0, s_Sizes.size(), 1)->Unit(benchmark::kMillisecond);
BENCHMARK(Allocate_FreeTreeAllocator)->DenseRange(0, s_Sizes.size(), 1)->Unit(benchmark::kMillisecond);
BENCHMARK(Allocate_BuddyAllocator)->DenseRange(0, s_Sizes.size(), 1)->Unit(benchmark::kMillisecond);

BENCHMARK(RandomAllocate_CAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocate_LinearAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocate_StackAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocate_FreeListAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocate_FreeTreeAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocate_BuddyAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);

BENCHMARK(RandomAllocateAndFree_CAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocateAndFree_LinearAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocateAndFree_StackAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(FixedAllocateAndRandomFree_PoolAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocateAndFree_FreeListAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocateAndFree_FreeTreeAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);
BENCHMARK(RandomAllocateAndFree_BuddyAllocator)->DenseRange(0, s_NumOfRandAllocations, 10000)->Unit(benchmark::kMillisecond);

int main(int argc, char** argv)
{
	SetUp();
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
}