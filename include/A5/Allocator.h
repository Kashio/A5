#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>

namespace A5
{
	class Allocator
	{
	public:
	protected:
		std::size_t m_Size;
		void* m_StartAddress;
	protected:
		Allocator(const std::size_t size)
			: m_Size(size)
		{}
	public:
		virtual ~Allocator() = default;
		virtual void* Allocate(const std::size_t size, const std::size_t alignment) = 0;
		virtual void Deallocate(void* ptr) = 0;
		virtual void Reset() = 0;
	};
};

#endif // !ALLOCATOR_H
