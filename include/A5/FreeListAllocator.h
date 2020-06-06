#ifndef FREE_LIST_ALLOCATOR_H
#define FREE_LIST_ALLOCATOR_H

#include "Allocator.h"

#include "LinkedList.h"

namespace A5
{
	class FreeListAllocator : public Allocator
	{
	public:
		enum class SearchMethod
		{
			FIRST,
			BEST
		};
	private:
		struct alignas(std::max_align_t) Header
		{
			std::size_t m_Size;
		};
		LinkedList m_List;
		SearchMethod m_SearchMethod;
	public:
		FreeListAllocator(const std::size_t size, const SearchMethod searchMethod);
		~FreeListAllocator() override;
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	private:
		void Init();
		void Coalescence(LinkedList::Node* prev, LinkedList::Node* curr);
	};
};

#endif // !FREE_LIST_ALLOCATOR_H
