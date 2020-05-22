#include "A5/Allocator.h"
#include "A5/LinearAllocator.h"
#include "A5/StackAllocator.h"
#include "A5/PoolAllocator.h"
#include "A5/FreeListAllocator.h"
#include "A5/RBTree.h"

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
		alloc.Deallocate(p);
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
	{
		auto alloc = A5::LinearAllocator(80);
		auto ptr0 = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
		auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
		auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
		std::cout << "ptr0[1].m_a: " << ptr0.get()[1].m_a << '\n';
		std::cout << "ptr1[0]: " << ptr1.get()[0] << '\n';
		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
	}
	{
		auto alloc = A5::StackAllocator(80);
		auto ptr0 = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
		auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
		auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
		alloc.Deallocate(ptr1.get());
		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
		std::cout << "ptr0[1].m_a: " << ptr0.get()[1].m_a << '\n';
		std::cout << "ptr1[0]: " << ptr1.get()[0] << '\n';
		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
	}
	{
		auto alloc = A5::PoolAllocator(80, sizeof(M), true);
		auto ptr0 = make_T_Construct<M>(alloc, 1, 'A', 11);
		auto ptr1 = make_T_Construct<M>(alloc, 1, 'B', 22);
		auto ptr2 = make_T_Construct<M>(alloc, 1, 'C', 33);
		auto ptr3 = make_T_Construct<M>(alloc, 1, 'D', 44);
		auto ptr4 = make_T_Construct<M>(alloc, 1, 'E', 55);
		auto ptr5 = make_T_Construct<M>(alloc, 1, 'F', 66);
		auto ptr6 = make_T_Construct<M>(alloc, 1, 'G', 77);
		auto ptr7 = make_T_Construct<M>(alloc, 1, 'H', 88);
		auto ptr8 = make_T_Construct<M>(alloc, 1, 'I', 99);
		auto ptr9 = make_T_Construct<M>(alloc, 1, 'J', 111);
		auto ptr10 = make_T_Construct<M>(alloc, 1, 'K', 222);
		alloc.Deallocate(ptr1.get());
		auto ptr11 = make_T_Construct<M>(alloc, 1, 'L', 333);
		auto ptr12 = make_T_Construct<M>(alloc, 1, 'M', 444);
		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
		std::cout << "ptr0[0].m_i: " << ptr0.get()[0].m_i << '\n';
		std::cout << "ptr1[0].m_a: " << ptr1.get()[0].m_a << '\n';
		std::cout << "ptr1[0].m_i: " << ptr1.get()[0].m_i << '\n';
		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
	}
	{
		std::cout << "XX: " << sizeof(long double) << '\n';
		auto alloc = A5::FreeListAllocator(80, true);
		auto ptr0 = make_T_Construct<L>(alloc, 2, 'a', 'b', 'c');
		auto ptr1 = make_T_Construct<char>(alloc, 1, 'Z');
		auto ptr2 = make_T_Construct<M>(alloc, 1, 'D', 5);
		alloc.Deallocate(ptr1.get());
		alloc.Deallocate(ptr0.get());
		auto ptr3 = make_T_Construct<double>(alloc, 1, 5.5);
		alloc.Deallocate(ptr3.get());
		std::cout << "ptr0[0].m_a: " << ptr0.get()[0].m_a << '\n';
		std::cout << "ptr0[1].m_a: " << ptr0.get()[1].m_a << '\n';
		std::cout << "ptr1[0]: " << ptr1.get()[0] << '\n';
		std::cout << "ptr2[0].m_a: " << ptr2.get()[0].m_a << '\n';
		std::cout << "ptr2[0].m_i: " << ptr2.get()[0].m_i << '\n';
	}
	{
		auto t = A5::RBTree(sizeof(A5::RBTree::Node) * 100);
		t.Insert(13);
		t.Insert(8);
		t.Insert(17);
		t.Insert(15);
		t.Insert(11);
		t.Insert(1);
		t.Insert(25);
		t.Insert(22);
		t.Insert(6);
		t.Insert(27);
		t.Remove(t.Search(8));
	}
	return 0;
}