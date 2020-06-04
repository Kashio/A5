#include <gtest/gtest.h>
#include<A5/Allocator.h>
#include <random>
#include <unordered_map>

constexpr std::size_t maxAlignment = alignof(std::max_align_t);

static unsigned int SelectRandomly(unsigned int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<std::mt19937::result_type> dis(0, max);
	return dis(gen);
}

static void FillRandomBytes(std::unordered_map<void*, std::vector<unsigned char>>& addresses, void* address, std::size_t count)
{
	if (address == nullptr)
		return;
	addresses[address] = std::vector<unsigned char>(count);
	for (unsigned int i = 0; i < count; ++i)
	{
		unsigned char byte = (unsigned char)SelectRandomly(255);
		addresses[address][i] = byte;
		*(reinterpret_cast<char*>(address) + i) = byte;
	}
}

static void RandomAllocate(A5::Allocator* alloc)
{
	std::vector<std::size_t> randomSizes;
	randomSizes.reserve(1000);
	for (std::size_t i = 0; i < randomSizes.capacity(); ++i)
	{
		randomSizes.push_back(SelectRandomly(512) + 1);
	}

	std::unordered_map<void*, std::vector<unsigned char>> addresses;
	addresses.reserve(1000);

	for (auto s : randomSizes)
	{
		void* address = alloc->Allocate(s, maxAlignment);
		FillRandomBytes(addresses, address, s);
	}
	for (auto a : addresses)
	{
		void* address = a.first;
		for (unsigned int i = 0; i < a.second.size(); ++i)
		{
			ASSERT_EQ(a.second[i], *(reinterpret_cast<unsigned char*>(address) + i));
		}
	}
}

static void RandomAllocateDeallocate(A5::Allocator* alloc)
{
	std::vector<std::size_t> randomSizes;
	randomSizes.reserve(1000);
	for (std::size_t i = 0; i < randomSizes.capacity(); ++i)
	{
		randomSizes.push_back(SelectRandomly(512) + 1);
	}

	std::unordered_map<void*, std::vector<unsigned char>> addresses;
	addresses.reserve(1000);

	std::vector<void*> addressesToDeallocate;
	for (auto s : randomSizes)
	{
		void* address = alloc->Allocate(s, maxAlignment);
		FillRandomBytes(addresses, address, s);
		if (SelectRandomly(100) <= 50)
			addressesToDeallocate.push_back(address);
	}
	for (auto a : addressesToDeallocate)
	{
		alloc->Deallocate(a);
		addresses.erase(a);
	}
	for (unsigned int i = 0; i < addressesToDeallocate.size(); ++i)
	{
		void* address = alloc->Allocate(randomSizes[i], maxAlignment);
		FillRandomBytes(addresses, address, randomSizes[i]);
	}
	for (auto a : addresses)
	{
		void* address = a.first;
		for (unsigned int i = 0; i < a.second.size(); ++i)
		{
			ASSERT_EQ(a.second[i], *(reinterpret_cast<unsigned char*>(address) + i));
		}
	}
}

static void RandomAllocateDeallocateLinearly(A5::Allocator* alloc)
{
	std::vector<std::size_t> randomSizes;
	randomSizes.reserve(1000);
	for (unsigned int i = 0; i < randomSizes.capacity(); ++i)
	{
		randomSizes.push_back(SelectRandomly(512) + 1);
	}

	std::unordered_map<void*, std::vector<unsigned char>> addresses;
	addresses.reserve(1000);

	std::vector<void*> addressesToDeallocate;
	addressesToDeallocate.reserve(SelectRandomly(100));
	for (unsigned int i = 0; i < randomSizes.size(); ++i)
	{
		void* address = alloc->Allocate(randomSizes[i], maxAlignment);
		FillRandomBytes(addresses, address, randomSizes[i]);
		if (i >= randomSizes.size() - addressesToDeallocate.capacity())
		{
			addressesToDeallocate.push_back(address);
		}
	}
	for (auto a : addressesToDeallocate)
	{
		alloc->Deallocate(a);
		addresses.erase(a);
	}
	for (unsigned int i = 0; i < addressesToDeallocate.size(); ++i)
	{
		void* address = alloc->Allocate(randomSizes[i], maxAlignment);
		FillRandomBytes(addresses, address, randomSizes[i]);
	}
	for (auto a : addresses)
	{
		void* address = a.first;
		for (int i = 0; i < a.second.size(); ++i)
		{
			ASSERT_EQ(a.second[i], *(reinterpret_cast<unsigned char*>(address) + i));
		}
	}
}

static void FixedAllocateDeallocate(A5::Allocator* alloc, const std::size_t size)
{
	std::unordered_map<void*, std::vector<unsigned char>> addresses;
	addresses.reserve(1000);

	std::vector<void*> addressesToDeallocate;
	for (unsigned int i = 0; i < 1000; ++i)
	{
		void* address = alloc->Allocate(size, maxAlignment);
		FillRandomBytes(addresses, address, size);
		if (SelectRandomly(100) <= 50)
			addressesToDeallocate.push_back(address);
	}
	for (auto a : addressesToDeallocate)
	{
		alloc->Deallocate(a);
		addresses.erase(a);
	}
	for (unsigned int i = 0; i < addressesToDeallocate.size(); ++i)
	{
		void* address = alloc->Allocate(size, maxAlignment);
		FillRandomBytes(addresses, address, size);
	}
	for (auto a : addresses)
	{
		void* address = a.first;
		for (unsigned int i = 0; i < a.second.size(); ++i)
		{
			ASSERT_EQ(a.second[i], *(reinterpret_cast<unsigned char*>(address) + i));
		}
	}
}