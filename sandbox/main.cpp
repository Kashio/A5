#include "A5/Allocator.h"
#include "A5/LinearAllocator.h"

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <cstddef>

struct L
{
	char m_a;
	char m_b;
	char m_c;

	L(char a, char b, char c)
		: m_a(a), m_b(b), m_c(c)
	{}

	L(const L& other)
	{
		m_a = other.m_a;
		m_b = other.m_b;
		m_c = other.m_c;
		std::cout << "Copy L()\n";
	}

	~L()
	{
		std::cout << "~L()\n";
	}
};

struct M
{
	char m_a;
	int m_i;

	M(char a, int i)
		: m_a(a), m_i(i)
	{}

	M(const M& other)
	{
		m_a = other.m_a;
		m_i = other.m_i;
		std::cout << "Copy M()\n";
	}

	~M()
	{
		std::cout << "~M()\n";
	}
};

template<typename T, typename Alloc, typename... Args>
std::unique_ptr<T[], std::function<void(T*)>> make_T_Construct(Alloc& alloc, std::size_t size, Args&&... args)
{
	T* ptr = reinterpret_cast<T*>(alloc.Allocate(size * sizeof(T), alignof(T)));

	for (std::size_t i = 0; i < size; ++i)
	{
		new(ptr + i) T(std::forward<Args>(args)...);
	}

	auto deleter = [](T* p, Alloc& alloc, std::size_t size)
	{
		for (std::size_t i = 0; i < size; ++i)
		{
			p[i].~T();
		}
		//alloc.Deallocate(p, sizeof(T) * size);
	};

	return { ptr, std::bind(deleter, std::placeholders::_1, std::ref(alloc), size) };
}

int main()
{
	std::cout << "Alignment of L: " << alignof(L) << '\n';
	std::cout << "Size of L: " << sizeof(L) << '\n';
	std::cout << "Size of L*: " << sizeof(L*) << '\n';
	std::cout << "Alignment of M: " << alignof(M) << '\n';
	std::cout << "Size of M: " << sizeof(M) << '\n';
	std::cout << "Size of M*: " << sizeof(M*) << '\n';
	auto alloc = A5::LinearAllocator(80);

	auto ptr = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
	auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
	auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
	std::cout << "ptr[0].m_a: " << ptr.get()[0].m_a << '\n';
	std::cout << "ptr[1].m_a: " << ptr.get()[1].m_a << '\n';
	return 0;
}