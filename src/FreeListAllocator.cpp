#include "A5/FreeListAllocator.h"

#include <memory>
#include <cassert>
#include <string>

A5::FreeListAllocator::FreeListAllocator(const std::size_t size)
	: Allocator(size), m_Head(nullptr)
{
	static std::string message = "Total size must be bigger than size of FreeListAllocator::Chunk for allocator with atleast " + std::to_string(sizeof(std::size_t)) + " bytes space";;
	assert(size > sizeof(Chunk) && message.c_str());
	m_StartAddress = ::operator new(size);
	m_Head = reinterpret_cast<Chunk*>(m_StartAddress);
	m_Head->m_Size = size - sizeof(Header);
	m_Head->m_Next = nullptr;
}

A5::FreeListAllocator::~FreeListAllocator()
{
	::operator delete(m_StartAddress);
	m_StartAddress = nullptr;
}

void* A5::FreeListAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	std::size_t sizePadding;
	std::size_t headerPadding;
	Chunk* prev;
	Chunk* curr;
	Find(size, alignment, sizePadding, headerPadding, prev, curr);

	if (curr == nullptr)
	{
		return nullptr;
	}

	if (curr->m_Size >= sizePadding + size + headerPadding + sizeof(Chunk*))
	{
		Chunk* splittedChunk = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(curr) + sizeof(Header) + sizePadding + size + headerPadding);
		splittedChunk->m_Size = curr->m_Size - (sizePadding + size + headerPadding + sizeof(Header));
		splittedChunk->m_Next = curr->m_Next;
		curr->m_Next = splittedChunk;
	}
	else
	{
		headerPadding += curr->m_Size - (sizePadding + size + headerPadding);
	}

	if (prev == nullptr)
	{
		m_Head = curr->m_Next;
	}
	else
	{
		prev->m_Next = curr->m_Next;
	}

	for (std::size_t i = 0; i < sizePadding; i += sizeof(std::size_t))
	{
		*reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(curr) + sizeof(Header) + i) = 1;
	}

	Header* header = reinterpret_cast<Header*>(curr);
	header->m_Size = sizePadding + size + headerPadding;

	return reinterpret_cast<char*>(curr) + sizeof(Header) + sizePadding;
}

void A5::FreeListAllocator::Deallocate(void* ptr)
{
	Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));
	while (header->m_Size == 1)
	{
		header = reinterpret_cast<Header*>(reinterpret_cast<std::size_t*>(header) - 1);
	}

	Chunk* chunk = reinterpret_cast<Chunk*>(header);
	chunk->m_Size = header->m_Size;

	Chunk* prevIt = nullptr;
	Chunk* it = m_Head;
	while (it != nullptr) {
		if (chunk < it) {
			chunk->m_Next = it;
			if (prevIt == nullptr)
			{
				m_Head = chunk;
			}
			else
			{
				prevIt->m_Next = chunk;
			}
			break;
		}
		prevIt = it;
		it = it->m_Next;
	}

	Coalescence(prevIt, chunk);
}

void A5::FreeListAllocator::Reset()
{
	m_Head = reinterpret_cast<Chunk*>(m_StartAddress);
	m_Head->m_Size = m_Size - sizeof(Header);
	m_Head->m_Next = nullptr;
}

void A5::FreeListAllocator::Find(const std::size_t size, const std::size_t alignment, std::size_t& sizePadding, std::size_t& headerPadding, Chunk*& prev, Chunk*& curr)
{
	headerPadding = size % sizeof(std::size_t) != 0 ? sizeof(std::size_t) - size % sizeof(std::size_t) : 0;

	Chunk* prevIt = nullptr;
	Chunk* it = m_Head;
	Chunk* prevBest = nullptr;
	Chunk* best = nullptr;

	while (it != nullptr)
	{
		void* currentAddress = reinterpret_cast<char*>(it) + sizeof(Header);
		std::size_t space = it->m_Size;
		std::align(alignment, size, currentAddress, space);
		sizePadding = reinterpret_cast<char*>(currentAddress) - reinterpret_cast<char*>(it) - sizeof(Header);
		if (it->m_Size >= sizePadding + size + headerPadding)
		{
			if (best == nullptr || it->m_Size < best->m_Size)
			{
				prevBest = prevIt;
				best = it;
			}
		}
		prevIt = it;
		it = it->m_Next;
	}

	prev = prevBest;
	curr = best;
}

void A5::FreeListAllocator::Coalescence(A5::FreeListAllocator::Chunk* prev, A5::FreeListAllocator::Chunk* curr)
{
	if (curr->m_Next != nullptr && (std::size_t)curr + curr->m_Size + sizeof(Header) == (std::size_t)curr->m_Next)
	{
		curr->m_Size += curr->m_Next->m_Size + sizeof(Header);
		curr->m_Next = curr->m_Next->m_Next;
	}

	if (prev != nullptr && (std::size_t)prev + prev->m_Size + sizeof(Header) == (std::size_t)curr)
	{
		prev->m_Size += curr->m_Size + sizeof(Header);
		prev->m_Next = curr->m_Next;
	}
}
