#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>

namespace A5
{
	class Allocator
	{
	public:
	protected:
		void* m_StartAddress;
		std::size_t m_Size;
		std::size_t m_Allocations;
	protected:
		Allocator(const std::size_t size)
			: m_Size(size)
		{
			m_StartAddress = ::operator new(size);
			m_Allocations = 0;
			//m_Alignment = sizeof(this);
			//m_Padding = -((int)sizeof(T)) & (m_Alignment - 1);
		}
	public:
		virtual ~Allocator() {};
		virtual void* Allocate(std::size_t size, std::size_t alignment) = 0;
		virtual void Deallocate(void* ptr) = 0;
		virtual void Reset() = 0;
		virtual std::size_t Fragmentation() = 0;
	};
};

#endif // !ALLOCATOR_H
