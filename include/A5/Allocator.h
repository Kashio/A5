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
	protected:
		Allocator(const std::size_t size)
			: m_Size(size)
		{
			m_StartAddress = ::operator new(size);
		}
	public:
		virtual ~Allocator()
		{
			::operator delete(m_StartAddress);
			m_StartAddress = nullptr;
		};
		virtual void* Allocate(const std::size_t size, const std::size_t alignment) = 0;
		virtual void Deallocate(void* ptr) = 0;
		virtual void Reset() = 0;
	};
};

#endif // !ALLOCATOR_H
