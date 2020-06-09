#include "A5/PoolAllocator.h"

#include <memory>
#include <cassert>

A5::PoolAllocator::PoolAllocator(const std::size_t size, const std::size_t chunkSize, const bool resizeable)
	: Allocator(size), m_ChunkSize(chunkSize), m_Head(nullptr), m_Resizeable(resizeable)
{
	assert(chunkSize >= sizeof(Chunk) && "Chunk size must be greater or equal to pointer size");
	assert(size % chunkSize == 0 && "Total size must be a multiple of chunk size");
	m_StartAddress = AllocateBlock();
	m_Head = reinterpret_cast<Chunk*>(m_StartAddress);
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

	if (m_Head == nullptr)
	{
		if (m_Resizeable)
		{
			if (m_CurrentBlock == m_Blocks.size())
			{
				m_Head = AllocateBlock();
				m_Blocks.push_back(m_Head);
				++m_CurrentBlock;
			}
			else
			{
				++m_CurrentBlock;
				m_Head = reinterpret_cast<Chunk*>(m_Blocks[m_CurrentBlock - 1]);
			}
		}
		else
		{
			return nullptr;
		}
	}

	Chunk* chunk = m_Head;
	if (chunk->m_Next == nullptr &&
		(std::size_t)reinterpret_cast<char*>(chunk) + m_ChunkSize != (std::size_t)reinterpret_cast<char*>(m_Blocks[m_CurrentBlock - 1]) + m_Size)
	{
		m_Head = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(chunk) + m_ChunkSize);
		m_Head->m_Next = nullptr;
	}
	else
	{
		m_Head = m_Head->m_Next;
	}

	return chunk;
}

void A5::PoolAllocator::Deallocate(void* ptr)
{
	reinterpret_cast<Chunk*>(ptr)->m_Next = m_Head;
	m_Head = reinterpret_cast<Chunk*>(ptr);
}

void A5::PoolAllocator::Reset()
{
	for (auto& block : m_Blocks)
	{
		reinterpret_cast<Chunk*>(block)->m_Next = nullptr;
	}
	m_Head = reinterpret_cast<Chunk*>(m_StartAddress);
	m_CurrentBlock = 1;
}

A5::PoolAllocator::Chunk* A5::PoolAllocator::AllocateBlock()
{
	Chunk* block = reinterpret_cast<Chunk*>(::operator new(m_Size));
	block->m_Next = nullptr;
	return block;
}
