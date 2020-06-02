#ifndef C_ALLOCATOR_H
#define C_ALLOCATOR_H

#include "Allocator.h"

namespace A5
{
	class CAllocator : public Allocator
	{
	public:
		CAllocator();
		~CAllocator() override = default;
		void* Allocate(const std::size_t size, const std::size_t alignment) override;
		void Deallocate(void* ptr) override;
		void Reset() override;
	};
};

#endif // !C_ALLOCATOR_H
