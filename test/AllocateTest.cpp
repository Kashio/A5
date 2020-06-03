#include <gtest/gtest.h>
#include <A5/LinearAllocator.h>
#include <A5/StackAllocator.h>
#include <A5/PoolAllocator.h>
#include <A5/FreeListAllocator.h>
#include <A5/FreeTreeAllocator.h>
#include <A5/BuddyAllocator.h>

#include <vector>
#include <unordered_map>

#include "Utils.cpp"

constexpr std::size_t mb = 1024 * 1024;

TEST(AllocateTest, LinearAllocatorAllocation)
{
	A5::LinearAllocator alloc(mb);
	RandomAllocate(&alloc);
}

TEST(AllocateTest, StackAllocatorAllocation)
{
	A5::StackAllocator alloc(mb);
	RandomAllocateDeallocateLinearly(&alloc);
}

TEST(AllocateTest, PoolAllocatorAllocation)
{
	A5::PoolAllocator alloc(mb, 512, true);
	FixedAllocateDeallocate(&alloc, 512);
}

TEST(AllocateTest, FreeListAllocatorAllocation)
{
	A5::FreeListAllocator alloc(mb);
	RandomAllocateDeallocate(&alloc);
}

TEST(AllocateTest, FreeTreeAllocatorAllocation)
{
	A5::FreeTreeAllocator alloc(mb);
	RandomAllocateDeallocate(&alloc);
}

TEST(AllocateTest, BuddyAllocatorAllocation)
{
	A5::BuddyAllocator alloc(mb);
	RandomAllocateDeallocate(&alloc);
}