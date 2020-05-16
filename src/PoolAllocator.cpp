#include "A5/PoolAllocator.h"

#include <memory>
#include <cassert>

A5::PoolAllocator::PoolAllocator(const std::size_t size, const std::size_t chunkSize, bool resizeable)
	: Allocator(size), m_ChunkSize(chunkSize), m_Head(nullptr), m_Resizeable(resizeable)
{
	assert(chunkSize >= sizeof(void*) && "Chunk size must be greater or equal to pointer size");
	assert(size % chunkSize == 0 && "Total size must be a multiple of chunk size");
	m_Head = AllocateBlock(chunkSize);
	m_StartAddress = m_Head;
	m_Blocks.push_back(m_StartAddress);
	m_CurrentBlock = 1;
}

A5::PoolAllocator::~PoolAllocator()
{
	for (auto& block : m_Blocks)
	{
		::operator delete(block);
	}
	m_StartAddress = nullptr;
}

void* A5::PoolAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	assert(size == m_ChunkSize && "Allocation size must be equal to chunk size");

	if (m_Head == nullptr && m_Resizeable && m_CurrentBlock == m_Blocks.size())
	{
		m_Head = AllocateBlock(size);
		m_Blocks.push_back(m_Head);
		++m_CurrentBlock;
	}

	Chunk* chunk = m_Head;
	m_Head = m_Head->m_Next;

	return chunk;
}

void A5::PoolAllocator::Deallocate(void* ptr)
{
	reinterpret_cast<Chunk*>(ptr)->m_Next = m_Head;
	m_Head = reinterpret_cast<Chunk*>(ptr);
}

void A5::PoolAllocator::Reset()
{
	m_Head = reinterpret_cast<Chunk*>(m_StartAddress);
	m_CurrentBlock = 1;
}

A5::PoolAllocator::Chunk* A5::PoolAllocator::AllocateBlock(const std::size_t chunkSize)
{
	Chunk* block = reinterpret_cast<Chunk*>(::operator new(m_Size));

	Chunk* chunk = block;

	for (int i = 0; i < m_Size / m_ChunkSize - 1; ++i) {
		chunk->m_Next = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + chunkSize);
		chunk = chunk->m_Next;
	}

	chunk->m_Next = nullptr;

	return block;
}
