#ifndef FREE_TREE_ALLOCATOR_H
#define FREE_TREE_ALLOCATOR_H

#include "Allocator.h"

#include "RBTree.h"

namespace A5
{
	class FreeTreeAllocator : public Allocator
	{
	private:
		struct Header
		{
			std::size_t m_PrevSize;
			std::size_t m_Size;
		};
		RBTree m_Tree;
	public:
		FreeTreeAllocator(const std::size_t size);
		~FreeTreeAllocator();
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	private:
		void Init();
		void Coalescence(RBTree::Node* curr);
		static std::size_t GetRootNodePadding();
	};
};

#endif // !FREE_TREE_ALLOCATOR_H
