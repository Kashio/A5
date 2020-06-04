#include <benchmark/benchmark.h>
#include "Common.h"
#include "A5/Allocator.h"
#include "A5/CAllocator.h"
#include "A5/LinearAllocator.h"
#include "A5/StackAllocator.h"
#include "A5/PoolAllocator.h"
#include "A5/FreeListAllocator.h"
#include "A5/FreeTreeAllocator.h"
#include "A5/BuddyAllocator.h"

static void BenchmarkMultipleAllocations(A5::Allocator* alloc, benchmark::State& state)
{
	for (auto _ : state)
	{
		for (auto s : s_Sizes)
		{
			benchmark::DoNotOptimize(alloc->Allocate(s, s_MaxAlignment));
		}
		state.PauseTiming();
		alloc->Reset();
		state.ResumeTiming();
	}
}

static void BenchmarkMultipleFixedAllocations(A5::Allocator* alloc, benchmark::State& state, const std::size_t size)
{
	for (auto _ : state)
	{
		for (auto s : s_Sizes)
		{
			benchmark::DoNotOptimize(alloc->Allocate(size, s_MaxAlignment));
		}
		state.PauseTiming();
		alloc->Reset();
		state.ResumeTiming();
	}
}

static void BenchmarkMultieRandomAllocations(A5::Allocator* alloc, benchmark::State& state)
{
	for (auto _ : state)
	{
		for (auto s : s_RandomSizes)
		{
			benchmark::DoNotOptimize(alloc->Allocate(s, s_MaxAlignment));
		}
		state.PauseTiming();
		alloc->Reset();
		state.ResumeTiming();
	}
}

static void BenchmarkMultieRandomAllocationsAndFrees(A5::Allocator* alloc, benchmark::State& state)
{
	auto it = s_DeallocationIndices.begin();
	std::vector<void*> addresses;

	for (auto _ : state)
	{
		for (std::size_t i = 0; i < s_RandomSizes.size(); ++i)
		{
			void* address;
			address = alloc->Allocate(s_RandomSizes[i], s_MaxAlignment);
			state.PauseTiming();
			if (*it == i)
			{
				addresses.push_back(address);
				++it;
			}
			state.ResumeTiming();
		}
		for (std::size_t i = 0; i < addresses.size(); ++i)
		{
			alloc->Deallocate(addresses[i]);
		}
		for (std::size_t i = 0; i < s_RandomSizes.size(); ++i)
		{
			benchmark::DoNotOptimize(alloc->Allocate(s_RandomSizes[i], s_MaxAlignment));
		}
		state.PauseTiming();
		alloc->Reset();
		addresses.clear();
		it = s_DeallocationIndices.begin();
		state.ResumeTiming();
	}
}

static void Allocate_CAllocator(benchmark::State& state)
{
	A5::CAllocator alloc;
	BenchmarkMultipleAllocations(&alloc, state);
}

static void Allocate_LinearAllocator(benchmark::State& state)
{
	A5::LinearAllocator alloc(s_30MB);
	BenchmarkMultipleAllocations(&alloc, state);
}

static void Allocate_StackAllocator(benchmark::State& state)
{
	A5::StackAllocator alloc(s_30MB);
	BenchmarkMultipleAllocations(&alloc, state);
}

static void Allocate_PoolAllocator(benchmark::State& state)
{
	A5::PoolAllocator alloc(s_30MB, s_MaxBlockSize, true);
	BenchmarkMultipleFixedAllocations(&alloc, state, s_MaxBlockSize);
}

static void Allocate_FreeListAllocator(benchmark::State& state)
{
	A5::FreeListAllocator alloc(s_30MB);
	BenchmarkMultipleAllocations(&alloc, state);
}

static void Allocate_FreeTreeAllocator(benchmark::State& state)
{
	A5::FreeTreeAllocator alloc(s_30MB);
	BenchmarkMultipleAllocations(&alloc, state);;
}

static void Allocate_BuddyAllocator(benchmark::State& state)
{
	A5::BuddyAllocator alloc(s_30MB);
	BenchmarkMultipleAllocations(&alloc, state);
}
