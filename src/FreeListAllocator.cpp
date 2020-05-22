#include "A5/FreeListAllocator.h"

#include <memory>
#include <cassert>

A5::FreeListAllocator::FreeListAllocator(const std::size_t size, bool resizeable)
	: Allocator(size), m_Head(nullptr), m_Resizeable(resizeable)
{
	m_StartAddress = ::operator new(size);
	m_Head = reinterpret_cast<Chunk*>(m_StartAddress);
	m_Head->m_Size = size - sizeof(Header);
	m_Head->m_Next = nullptr;
	m_Blocks.push_back(m_StartAddress);
	m_CurrentBlock = 1;
}

A5::FreeListAllocator::~FreeListAllocator()
{
	for (auto& block : m_Blocks)
	{
		::operator delete(block);
	}
	m_StartAddress = nullptr;
}

void* A5::FreeListAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	if (m_Head == nullptr && m_Resizeable && m_CurrentBlock == m_Blocks.size())
	{
		m_Head = reinterpret_cast<Chunk*>(::operator new(m_Size));
		m_Head->m_Size = m_Size - sizeof(Header);
		m_Head->m_Next = nullptr;
		m_Blocks.push_back(m_Head);
		++m_CurrentBlock;
	}

	std::size_t sizePadding;
	std::size_t headerPadding;
	Chunk* prev;
	Chunk* curr;
	Find(size, alignment, sizePadding, headerPadding, prev, curr);

	if (curr == nullptr)
	{
		return nullptr;
	}

	if (curr->m_Size > sizePadding + size + headerPadding)
	{
		Chunk* splittedChunk = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(curr) + sizeof(Header) + sizePadding + size + headerPadding);
		splittedChunk->m_Size = curr->m_Size - (sizePadding + size + headerPadding + sizeof(Header));
		splittedChunk->m_Next = curr->m_Next;
		curr->m_Next = splittedChunk;
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
	m_CurrentBlock = 1;
}

void A5::FreeListAllocator::Find(const std::size_t size, const std::size_t alignment, std::size_t& sizePadding, std::size_t& headerPadding, Chunk*& prev, Chunk*& curr)
{
	Chunk* prevIt = nullptr;
	Chunk* it = m_Head;

	while (it != nullptr)
	{
		void* currentAddress = reinterpret_cast<char*>(it) + sizeof(Header);
		std::size_t space = it->m_Size;
		std::align(alignment, size, currentAddress, space);
		sizePadding = reinterpret_cast<char*>(currentAddress) - reinterpret_cast<char*>(it) - sizeof(Header);
		headerPadding = size % sizeof(std::size_t) != 0 ? sizeof(std::size_t) - size % sizeof(std::size_t) : 0;
		if (it->m_Size >= sizePadding + size + headerPadding)
		{
			break;
		}
		prevIt = it;
		it = it->m_Next;
	}

	prev = prevIt;
	curr = it;
}

void A5::FreeListAllocator::Coalescence(A5::FreeListAllocator::Chunk* prev, A5::FreeListAllocator::Chunk* current)
{
	if (current->m_Next != nullptr && (std::size_t)current + current->m_Size + sizeof(Header) == (std::size_t)current->m_Next)
	{
		current->m_Size += current->m_Next->m_Size + sizeof(Header);
		current->m_Next = current->m_Next->m_Next;
	}

	if (prev != nullptr && (std::size_t)prev + prev->m_Size + sizeof(Header) == (std::size_t)current)
	{
		prev->m_Size += current->m_Size + sizeof(Header);
		prev->m_Next = current->m_Next;
	}
}
