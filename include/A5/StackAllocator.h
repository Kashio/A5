#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "Allocator.h"

namespace A5
{
	class StackAllocator : public Allocator
	{
	public:
		std::size_t m_Offset;

		StackAllocator(const std::size_t size);

		~StackAllocator() override;

		void* Allocate(const std::size_t size, const std::size_t alignment) override;

		void Deallocate(void* ptr) override;

		void Reset() override;

		std::size_t Fragmentation() override;
	private:
		using Header = unsigned char;
	};
};

#endif // !STACK_ALLOCATOR_H
