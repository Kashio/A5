#include "A5/CAllocator.h"

A5::CAllocator::CAllocator()
	: Allocator(0)
{
}

void* A5::CAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	return ::operator new(size);
}

void A5::CAllocator::Deallocate(void* ptr)
{
	::operator delete(ptr);
}

void A5::CAllocator::Reset()
{
}
