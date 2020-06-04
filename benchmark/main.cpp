#include <benchmark/benchmark.h>
#include "Common.h"
#include "Allocations.cpp"

static void SetUp()
{
	s_Sizes = { 1, 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 4096, 8192 };
	s_RandomSizes.reserve(s_NumOfRandAllocations);
	for (std::size_t i = 0; i < s_RandomSizes.capacity(); ++i)
	{
		s_RandomSizes.push_back(Utils::SelectRandomly(1, s_MaxBlockSize));
		if (Utils::SelectRandomly(1, 4) == 1)
			s_DeallocationIndices.push_back(i);
	}
}

BENCHMARK(Allocate_CAllocator);
BENCHMARK(Allocate_LinearAllocator);
BENCHMARK(Allocate_StackAllocator);
BENCHMARK(Allocate_PoolAllocator);
BENCHMARK(Allocate_FreeListAllocator);
BENCHMARK(Allocate_FreeTreeAllocator);
BENCHMARK(Allocate_BuddyAllocator);

int main(int argc, char** argv)
{
	SetUp();
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
}