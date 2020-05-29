#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "Allocator.h"

#include <vector>
#include <unordered_map>

namespace A5
{
	class BuddyAllocator : public Allocator
	{
	private:
		struct Boundry
		{
			std::size_t m_LowerBound;
			std::size_t m_UpperBound;
		};
		std::vector<std::vector<Boundry>> m_FreeLists;
		std::unordered_map<std::size_t, std::size_t> m_BlockSize;
	public:
		BuddyAllocator(const std::size_t size);
		~BuddyAllocator();
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	private:
		void Init();
	};
};

#endif // !BUDDY_ALLOCATOR_H
