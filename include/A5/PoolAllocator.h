#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "Allocator.h"

#include <vector>

namespace A5
{
	class PoolAllocator : public Allocator
	{
	private:
		struct Chunk
		{
			Chunk* m_Next;
		};
		std::size_t m_ChunkSize;
		Chunk* m_Head;
		std::size_t m_CurrentBlock;
		std::vector<void*> m_Blocks;
		bool m_Resizeable;
	public:
		PoolAllocator(const std::size_t size, const std::size_t chunkSize, const bool resizeable);
		~PoolAllocator() override;
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	private:
		Chunk* AllocateBlock();
	};
};

#endif // !POOL_ALLOCATOR_H
