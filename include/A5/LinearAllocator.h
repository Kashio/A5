#ifndef LINEARALLOCATOR_H
#define LINEARALLOCATOR_H

#include "Allocator.h"

namespace A5
{
	class LinearAllocator : public Allocator
	{
	public:
		std::size_t m_Offset;

		LinearAllocator(const std::size_t size);

		~LinearAllocator() override;

		void* Allocate(std::size_t size, std::size_t alignment) override;

		void Deallocate(void* ptr) override;

		void Reset() override;

		std::size_t Fragmentation() override;
	};
};

#endif // !LINEARALLOCATOR_H
