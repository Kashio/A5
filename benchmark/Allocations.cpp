#include <benchmark/benchmark.h>
#include "A5/Allocator.h"
#include "A5/CAllocator.h"
#include "A5/LinearAllocator.h"
#include "A5/StackAllocator.h"
#include "A5/PoolAllocator.h"
#include "A5/FreeListAllocator.h"
#include "A5/FreeTreeAllocator.h"
#include "A5/BuddyAllocator.h"
#include "Utils.cpp"

#include <iostream>

class MultipleFixedAllocations : public benchmark::Fixture {
private:
	static constexpr std::size_t s_MaxAlignment = alignof(std::max_align_t);
	static constexpr std::size_t s_NumOfRandAllocations = 1000;
	static constexpr std::size_t s_MaxBlockSize = 8192;
protected:
	static constexpr std::size_t s_30MB = 1024 * 1024 * 30;
	static std::vector<std::size_t> s_Sizes;
	static std::vector<std::size_t> s_RandomSizes;
	static std::vector<std::size_t> s_DeallocationIndices;
public:
	void SetUp(const ::benchmark::State& state)
	{
		s_Sizes = { 1, 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 4096, 8192 };
		s_RandomSizes.reserve(s_NumOfRandAllocations);
		for (std::size_t i = 0; i < s_RandomSizes.capacity(); ++i)
		{
			s_RandomSizes.push_back(SelectRandomly(1, s_MaxBlockSize));
			if (SelectRandomly(1, 4) == 1)
				s_DeallocationIndices.push_back(i);
		}
	}

	void TearDown(const ::benchmark::State& state)
	{
	}
protected:
	void BenchmarkMultipleAllocations(A5::Allocator* alloc, benchmark::State& state)
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

	void BenchmarkMultipleFixedAllocations(A5::Allocator* alloc, benchmark::State& state, const std::size_t size)
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

	void BenchmarkMultieRandomAllocations(A5::Allocator* alloc, benchmark::State& state)
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

	void BenchmarkMultieRandomAllocationsAndFrees(A5::Allocator* alloc, benchmark::State& state)
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
};

std::vector<std::size_t> MultipleFixedAllocations::s_Sizes;
std::vector<std::size_t> MultipleFixedAllocations::s_RandomSizes;
std::vector<std::size_t> MultipleFixedAllocations::s_DeallocationIndices;

BENCHMARK_F(MultipleFixedAllocations, CAllocator)(benchmark::State& state)
{
	A5::CAllocator alloc;
	this->BenchmarkMultipleAllocations(&alloc, state);
}

BENCHMARK_F(MultipleFixedAllocations, LinearAllocator)(benchmark::State& state)
{
	A5::LinearAllocator alloc(MultipleFixedAllocations::s_30MB);
	this->BenchmarkMultipleAllocations(&alloc, state);
}

BENCHMARK_F(MultipleFixedAllocations, StackAllocator)(benchmark::State& state)
{
	A5::StackAllocator alloc(MultipleFixedAllocations::s_30MB);
	this->BenchmarkMultipleAllocations(&alloc, state);
}

BENCHMARK_F(MultipleFixedAllocations, PoolAllocator)(benchmark::State& state)
{
	A5::PoolAllocator alloc(MultipleFixedAllocations::s_30MB, 8192, true);
	this->BenchmarkMultipleFixedAllocations(&alloc, state, 8192);
}

BENCHMARK_F(MultipleFixedAllocations, FreeListAllocator)(benchmark::State& state)
{
	A5::FreeListAllocator alloc(MultipleFixedAllocations::s_30MB);
	this->BenchmarkMultipleAllocations(&alloc, state);
}

BENCHMARK_F(MultipleFixedAllocations, FreeTreeAllocator)(benchmark::State& state)
{
	A5::FreeTreeAllocator alloc(MultipleFixedAllocations::s_30MB);
	this->BenchmarkMultipleAllocations(&alloc, state);
}