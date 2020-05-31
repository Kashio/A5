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
			char* m_LowerBound;
			char* m_UpperBound;

			Boundry(char* lowerBound, char* upperBound)
				: m_LowerBound(lowerBound), m_UpperBound(upperBound)
			{}
		};
		std::vector<std::vector<Boundry>> m_FreeLists;
		std::unordered_map<char*, std::size_t> m_BlockSize;
	public:
		BuddyAllocator(const std::size_t size);
		~BuddyAllocator();
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	private:
		void Init(const std::size_t x);
	};
};

#endif // !BUDDY_ALLOCATOR_H
