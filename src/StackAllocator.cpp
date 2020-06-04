#include "A5/StackAllocator.h"

#include <memory>

A5::StackAllocator::StackAllocator(const std::size_t size)
	: Allocator(size), m_Offset(0)
{
	m_StartAddress = ::operator new(size);
}

A5::StackAllocator::~StackAllocator()
{
	::operator delete(m_StartAddress);
	m_StartAddress = nullptr;
}

void* A5::StackAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	void* currentAddress = reinterpret_cast<char*>(m_StartAddress) + m_Offset;
	void* nextAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(currentAddress) + sizeof(Header));
	std::size_t space = m_Size - m_Offset - sizeof(Header);
	std::align(alignment, size, nextAddress, space);

	if ((std::size_t)nextAddress + size > (std::size_t)m_StartAddress + m_Size)
		return nullptr;

	std::size_t padding = (std::size_t)nextAddress - (std::size_t)currentAddress;

	Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(nextAddress) - sizeof(Header));
	*header = (Header)padding;

	m_Offset = (std::size_t)nextAddress - (std::size_t)m_StartAddress + size;

	return nextAddress;
}

void A5::StackAllocator::Deallocate(void* ptr)
{
	const std::size_t currentAddress = (std::size_t)ptr;
	Header* header = reinterpret_cast<Header*>(currentAddress - sizeof(Header));

	m_Offset = currentAddress - (std::size_t)m_StartAddress - *header;
}

void A5::StackAllocator::Reset()
{
	m_Offset = 0;
}
