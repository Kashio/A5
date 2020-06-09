#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "Allocator.h"

#include <array>
#include <limits>

#include "Utils.h"

namespace A5
{
	class BuddyAllocator : public Allocator
	{
	private:
		struct Node
		{
			Node* m_Next;
		};
		struct alignas(std::max_align_t) Header
		{
			std::size_t m_Size;
		};
		static constexpr std::size_t s_Log2Header = Utils::Log2(sizeof(Header));
		std::array<Node*, std::numeric_limits<std::size_t>::digits - s_Log2Header> m_Buckets = {};
	public:
		BuddyAllocator(const std::size_t size);
		~BuddyAllocator() override;
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	private:
		void Init();
	};
};

#endif // !BUDDY_ALLOCATOR_H
