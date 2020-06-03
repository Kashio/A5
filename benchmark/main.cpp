#include <benchmark/benchmark.h>
#include "A5/Allocator.h"
#include "A5/CAllocator.h"
#include "A5/LinearAllocator.h"
#include "A5/StackAllocator.h"
#include "A5/PoolAllocator.h"
#include "A5/FreeListAllocator.h"
#include "A5/FreeTreeAllocator.h"
#include "A5/BuddyAllocator.h"

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <cstddef>
#include <random>
#include <iterator>

static constexpr std::size_t maxAlignment = alignof(std::max_align_t);
static constexpr std::size_t mb = 1048576;

template<typename T, typename Alloc, typename... Args>
std::unique_ptr<T[], std::function<void(T*)>> make_T_Construct(Alloc& alloc, std::size_t size, Args&&... args)
{
	T* ptr = reinterpret_cast<T*>(alloc.Allocate(size * sizeof(T), alignof(T)));

	for (std::size_t i = 0; i < size; ++i)
	{
		new(ptr + i) T(std::forward<Args>(args)...);
	}

	auto deleter = [](T* p, Alloc& alloc, std::size_t size)
	{
		for (std::size_t i = 0; i < size; ++i)
		{
			p[i].~T();
		}
		alloc.Deallocate(p);
	};

	return { ptr, std::bind(deleter, std::placeholders::_1, std::ref(alloc), size) };
}

std::unique_ptr<A5::Allocator> GetAllocator(const std::size_t i)
{
	static constexpr std::size_t mb30 = 1048576 * 30;
	switch (i)
	{
	case 0:
		return std::make_unique<A5::CAllocator>();
	case 1:
		return std::make_unique<A5::LinearAllocator>(mb30);
	case 2:
		return std::make_unique<A5::StackAllocator>(mb30);
	case 3:
		return std::make_unique<A5::PoolAllocator>(8192000, 8192, true);
	case 4:
		return std::make_unique<A5::FreeListAllocator>(mb30);
	case 5:
		return std::make_unique<A5::FreeTreeAllocator>(mb30);
	case 6:
		return std::make_unique<A5::BuddyAllocator>(mb30);
	}
}

unsigned int SelectRandomly(unsigned int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<std::mt19937::result_type> dis(1, max);
	return dis(gen);
}

void BenchmarkMultipleAllocations(A5::Allocator* alloc, benchmark::State& state)
{
	static std::vector<std::size_t> sizes = { 1, 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 4096, 8192 };
	static constexpr std::size_t maxAlignment = alignof(std::max_align_t);

	for (auto _ : state)
	{
		for (auto s : sizes)
		{
			benchmark::DoNotOptimize(alloc->Allocate(s, maxAlignment));
		}
		state.PauseTiming();
		alloc->Reset();
		state.ResumeTiming();
	}
}

void BenchamrkMultipleFixedAllocations(A5::Allocator* alloc, benchmark::State& state, const std::size_t size)
{
	static std::vector<std::size_t> sizes = { 1, 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 4096, 8192 };
	static constexpr std::size_t maxAlignment = alignof(std::max_align_t);

	for (auto _ : state)
	{
		for (auto s : sizes)
		{
			benchmark::DoNotOptimize(alloc->Allocate(size, maxAlignment));
		}
		state.PauseTiming();
		alloc->Reset();
		state.ResumeTiming();
	}
}

void BM_MultipleFrees(benchmark::State& state)
{
	static constexpr std::size_t maxAlignment = alignof(std::max_align_t);
	std::vector<std::size_t> randomSizes;
	randomSizes.reserve(1000);
	for (std::size_t i = 0; i < randomSizes.capacity(); ++i)
	{
		randomSizes.push_back(SelectRandomly(8192));
	}

	std::vector<void*> addresses;

	auto r = state.range();

	auto object = GetAllocator(r);

	for (auto _ : state)
	{
		state.PauseTiming();
		for (auto s : randomSizes)
		{
			if (r != 3)
				addresses.push_back(object->Allocate(s, maxAlignment));
			else
				addresses.push_back(object->Allocate(8192, maxAlignment));
		}
		state.ResumeTiming();
		for (std::size_t i = 0; i < addresses.size(); ++i)
		{
			object->Deallocate(addresses[i]);
		}
		state.PauseTiming();
		object->Reset();
		addresses.clear();
		state.ResumeTiming();
	}
}

void BM_MultipleRandomAllocations(benchmark::State& state)
{
	static constexpr std::size_t maxAlignment = alignof(std::max_align_t);
	std::vector<std::size_t> randomSizes;
	randomSizes.reserve(200);
	for (std::size_t i = 0; i < randomSizes.capacity(); ++i)
	{
		randomSizes.push_back(SelectRandomly(8192));
	}

	auto r = state.range();

	auto object = GetAllocator(r);

	for (auto _ : state)
	{
		for (auto s : randomSizes)
		{
			if (r != 3)
				benchmark::DoNotOptimize(object->Allocate(s, maxAlignment));
			else
				benchmark::DoNotOptimize(object->Allocate(8192, maxAlignment));
		}
		state.PauseTiming();
		object->Reset();
		state.ResumeTiming();
	}
}

void BM_MultipleRandomAllocationsAndFrees(benchmark::State& state)
{
	static constexpr std::size_t maxAlignment = alignof(std::max_align_t);
	std::vector<std::size_t> randomSizes;
	randomSizes.reserve(1000);
	for (std::size_t i = 0; i < randomSizes.capacity(); ++i)
	{
		randomSizes.push_back(SelectRandomly(8192));
	}

	std::vector<void*> addresses;

	auto r = state.range();

	auto object = GetAllocator(r);

	for (auto _ : state)
	{
		for (std::size_t i = 0; i < randomSizes.size() / 2; ++i)
		{
			void* address;
			if (r != 3)
				address = object->Allocate(randomSizes[i], maxAlignment);
			else
				address = object->Allocate(8192, maxAlignment);
			state.PauseTiming();
			if (SelectRandomly(100) <= 25)
				addresses.push_back(address);
			state.ResumeTiming();
		}
		for (std::size_t i = 0; i < addresses.size(); ++i)
		{
			object->Deallocate(addresses[i]);
		}
		for (std::size_t i = randomSizes.size() / 2; i < randomSizes.size(); ++i)
		{
			if (r != 3)
				benchmark::DoNotOptimize(object->Allocate(randomSizes[i], maxAlignment));
			else
				benchmark::DoNotOptimize(object->Allocate(8192, maxAlignment));
		}
		state.PauseTiming();
		object->Reset();
		addresses.clear();
		state.ResumeTiming();
	}
}

void BM_MultipleAllocations_CAllocator(benchmark::State& state)
{
	A5::CAllocator alloc;
	BenchmarkMultipleAllocations(&alloc, state);
}

void BM_MultipleAllocations_LinearAllocator(benchmark::State& state)
{
	A5::LinearAllocator alloc(mb);
	BenchmarkMultipleAllocations(&alloc, state);
}

void BM_MultipleAllocations_StackAllocator(benchmark::State& state)
{
	A5::StackAllocator alloc(mb);
	BenchmarkMultipleAllocations(&alloc, state);
}

void BM_MultipleAllocations_PoolAllocator(benchmark::State& state)
{
	A5::PoolAllocator alloc(mb, 8192, true);
	BenchamrkMultipleFixedAllocations(&alloc, state, 8192);
}

void BM_MultipleAllocations_FreeListAllocator(benchmark::State& state)
{
	A5::FreeListAllocator alloc(mb);
	BenchmarkMultipleAllocations(&alloc, state);
}

void BM_MultipleAllocations_FreeTreeAllocator(benchmark::State& state)
{
	A5::FreeTreeAllocator alloc(mb);
	BenchmarkMultipleAllocations(&alloc, state);
}

void BM_MultipleAllocations_BuddyAllocator(benchmark::State& state)
{
	A5::BuddyAllocator alloc(mb);
	BenchmarkMultipleAllocations(&alloc, state);
}

//BENCHMARK(BM_MultipleAllocations)->DenseRange(0, 6);
//BENCHMARK(BM_MultipleFrees)->DenseRange(0, 6);
//BENCHMARK(BM_MultipleRandomAllocations)->DenseRange(0, 6);
//BENCHMARK(BM_MultipleRandomAllocationsAndFrees)->DenseRange(0, 6);

BENCHMARK(BM_MultipleAllocations_CAllocator);
BENCHMARK(BM_MultipleAllocations_LinearAllocator);
BENCHMARK(BM_MultipleAllocations_StackAllocator);
BENCHMARK(BM_MultipleAllocations_PoolAllocator);
BENCHMARK(BM_MultipleAllocations_FreeListAllocator);
BENCHMARK(BM_MultipleAllocations_FreeTreeAllocator);
BENCHMARK(BM_MultipleAllocations_BuddyAllocator);