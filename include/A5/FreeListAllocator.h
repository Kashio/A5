#ifndef FREE_LIST_ALLOCATOR_H
#define FREE_LIST_ALLOCATOR_H

#include "Allocator.h"

#include <vector>

namespace A5
{
	class FreeListAllocator : public Allocator
	{
	private:
		struct Header
		{
			std::size_t m_Size;
		};
		struct Chunk
		{
			std::size_t m_Size;
			Chunk* m_Next;
		};
		Chunk* m_Head;
		std::size_t m_CurrentBlock;
		std::vector<void*> m_Blocks;
		bool m_Resizeable;
	public:
		FreeListAllocator(const std::size_t size, bool resizeable);
		~FreeListAllocator();
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	private:
		void Find(const std::size_t size, const std::size_t alignment, std::size_t& sizePadding, std::size_t& headerPadding, Chunk*& prev, Chunk*& curr);
		void Coalescence(Chunk* prev, Chunk* current);
		Chunk* AllocateBlock(const std::size_t chunkSize);
	};
};

#endif // !FREE_LIST_ALLOCATOR_H
