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
#include  <random>
#include  <iterator>

struct L
{
	char m_a;
	char m_b;
	char m_c;

	L(char a, char b, char c)
		: m_a(a), m_b(b), m_c(c)
	{}

	L(const L& other)
	{
		m_a = other.m_a;
		m_b = other.m_b;
		m_c = other.m_c;
		std::cout << "Copy L()\n";
	}

	~L()
	{
		std::cout << "~L()\n";
	}
};

struct M
{
	char m_a;
	int m_i;

	M(char a, int i)
		: m_a(a), m_i(i)
	{}

	M(const M& other)
	{
		m_a = other.m_a;
		m_i = other.m_i;
		std::cout << "Copy M()\n";
	}

	~M()
	{
		std::cout << "~M()\n";
	}
};

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
	static constexpr std::size_t mb = 1048576 * 30;
	switch (i)
	{
	case 0:
		return std::make_unique<A5::CAllocator>();
	case 1:
		return std::make_unique<A5::LinearAllocator>(mb);
	case 2:
		return std::make_unique<A5::StackAllocator>(mb);
	case 3:
		return std::make_unique<A5::PoolAllocator>(8192000, 8192, true);
	case 4:
		return std::make_unique<A5::FreeListAllocator>(mb);
	case 5:
		return std::make_unique<A5::FreeTreeAllocator>(mb);
	case 6:
		return std::make_unique<A5::BuddyAllocator>(mb);
	}
}

void BM_MultipleAllocations(benchmark::State& state)
{
	static std::vector<std::size_t> sizes = { 1, 2, 4, 8, 16, 32, 64, 256, 512, 1024, 2048, 4096, 8192 };
	static constexpr std::size_t maxAlignment = alignof(std::max_align_t);

	auto r = state.range();

	auto object = GetAllocator(r);

	for (auto _ : state)
	{
		for (auto s : sizes)
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

unsigned int SelectRandomly(unsigned int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<std::mt19937::result_type> dis(1, max);
	return dis(gen);
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

BENCHMARK(BM_MultipleAllocations)->DenseRange(0, 6);
BENCHMARK(BM_MultipleRandomAllocations)->DenseRange(0, 6);
BENCHMARK(BM_MultipleRandomAllocationsAndFrees)->DenseRange(0, 6);

//int main()
//{
//	std::cout << "Alignment of L: " << alignof(L) << '\n';
//	std::cout << "Size of L: " << sizeof(L) << '\n';
//	std::cout << "Size of L*: " << sizeof(L*) << '\n';
//	std::cout << "Alignment of M: " << alignof(M) << '\n';
//	std::cout << "Size of M: " << sizeof(M) << '\n';
//	std::cout << "Size of M*: " << sizeof(M*) << '\n';
//	{
//		auto alloc = A5::LinearAllocator(80);
//		auto ptr0 = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
//		auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
//		auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
//		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
//		std::cout << "ptr0[1].m_a: " << ptr0.get()[1].m_a << '\n';
//		std::cout << "ptr1[0]: " << ptr1.get()[0] << '\n';
//		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
//		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
//	}
//	{
//		auto alloc = A5::StackAllocator(80);
//		auto ptr0 = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
//		auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
//		auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
//		//alloc.Deallocate(ptr1.get());
//		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
//		std::cout << "ptr0[1].m_a: " << ptr0.get()[1].m_a << '\n';
//		std::cout << "ptr1[0]: " << ptr1.get()[0] << '\n';
//		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
//		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
//	}
//	{
//		auto alloc = A5::PoolAllocator(80, sizeof(M), true);
//		auto ptr0 = make_T_Construct<M>(alloc, 1, 'A', 11);
//		auto ptr1 = make_T_Construct<M>(alloc, 1, 'B', 22);
//		auto ptr2 = make_T_Construct<M>(alloc, 1, 'C', 33);
//		auto ptr3 = make_T_Construct<M>(alloc, 1, 'D', 44);
//		auto ptr4 = make_T_Construct<M>(alloc, 1, 'E', 55);
//		auto ptr5 = make_T_Construct<M>(alloc, 1, 'F', 66);
//		auto ptr6 = make_T_Construct<M>(alloc, 1, 'G', 77);
//		auto ptr7 = make_T_Construct<M>(alloc, 1, 'H', 88);
//		auto ptr8 = make_T_Construct<M>(alloc, 1, 'I', 99);
//		auto ptr9 = make_T_Construct<M>(alloc, 1, 'J', 111);
//		auto ptr10 = make_T_Construct<M>(alloc, 1, 'K', 222);
//		//alloc.Deallocate(ptr1.get());
//		auto ptr11 = make_T_Construct<M>(alloc, 1, 'L', 333);
//		auto ptr12 = make_T_Construct<M>(alloc, 1, 'M', 444);
//		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
//		std::cout << "ptr0[0].m_i: " << ptr0.get()[0].m_i << '\n';
//		std::cout << "ptr1[0].m_a: " << ptr1.get()[0].m_a << '\n';
//		std::cout << "ptr1[0].m_i: " << ptr1.get()[0].m_i << '\n';
//		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
//		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
//	}
//	{
//		std::cout << "XX: " << sizeof(long double) << '\n';
//		auto alloc = A5::FreeListAllocator(80);
//		auto ptr0 = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
//		auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
//		auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
//		/*alloc.Deallocate(ptr1.get());
//		alloc.Deallocate(ptr0.get());*/
//		auto ptr3 = make_T_Construct<double>(alloc, 1, 5.5);
//		//alloc.Deallocate(ptr3.get());
//		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
//		std::cout << "ptr0[1].m_a: " << ptr0.get()[1].m_a << '\n';
//		std::cout << "ptr1[0]: " << ptr1.get()[0] << '\n';
//		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
//		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
//	}
//	{
//		auto alloc = A5::FreeTreeAllocator(200);
//		auto ptr0 = make_T_Construct<char>(alloc, 34, 'a');
//		auto ptr1 = make_T_Construct<char>(alloc, 3, 'b');
//		auto ptr3 = make_T_Construct<char>(alloc, 16, 'c');
//		/*auto ptr0 = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
//		auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
//		auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
//		alloc.Deallocate(ptr0.get());
//		alloc.Deallocate(ptr2.get());
//		alloc.Deallocate(ptr1.get());*/
//	}
//	{
//		//auto t = A5::RBTree(sizeof(A5::RBTree::Node) * 100);
//		//t.Insert(13);
//		//t.Insert(8);
//		//t.Insert(17);
//		//t.Insert(15);
//		//t.Insert(11);
//		//t.Insert(1);
//		//t.Insert(25);
//		//t.Insert(22);
//		//t.Insert(6);
//		//t.Insert(27);
//		//t.Remove(t.Search(8));
//	}
//	return 0;
//}