#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include "Allocator.h"

namespace A5
{
	class LinearAllocator : public Allocator
	{
	private:
		std::size_t m_Offset;
	public:
		LinearAllocator(const std::size_t size);
		~LinearAllocator() override;
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	};
};

#endif // !LINEAR_ALLOCATOR_H
