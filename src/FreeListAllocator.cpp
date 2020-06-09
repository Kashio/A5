#include "A5/FreeListAllocator.h"

#include <memory>
#include <cassert>
#include <string>

A5::FreeListAllocator::FreeListAllocator(const std::size_t size, const SearchMethod searchMethod)
	: Allocator(size)
{
	static std::string message = "Total size must be atleast " + std::to_string(sizeof(LinkedList::Node) + 1) + " bytes for an allocator with atleast 1 byte of free space";
	assert(size >= sizeof(LinkedList::Node) + 1 && message.c_str());
	m_SearchMethod = searchMethod;
	m_StartAddress = ::operator new(size);
	Init();
}

A5::FreeListAllocator::~FreeListAllocator()
{
	::operator delete(m_StartAddress);
	m_StartAddress = nullptr;
}

void* A5::FreeListAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	std::size_t padding;
	void* currentAddress = (void*)(sizeof(Header) + size);
	void* nextAddress = (void*)(sizeof(Header) + size);
	std::size_t space = size + 100;
	std::align(alignof(std::max_align_t), sizeof(std::max_align_t), nextAddress, space);
	padding = (std::size_t)nextAddress - (std::size_t)currentAddress;

	LinkedList::Node* prev;
	LinkedList::Node* best;

	switch (m_SearchMethod)
	{
	case SearchMethod::FIRST:
		m_List.SearchFirst(size + padding, best, prev);
		break;
	case SearchMethod::BEST:
		m_List.SearchBest(size + padding, best, prev);
		break;
	}

	if (best == nullptr)
	{
		return nullptr;
	}

	if (best->m_Value >= size + padding + sizeof(LinkedList::Node*) + 1)
	{
		LinkedList::Node* splittedNode = reinterpret_cast<LinkedList::Node*>(reinterpret_cast<char*>(best) + sizeof(Header) + size + padding);
		splittedNode->m_Value = best->m_Value - (size + padding + sizeof(Header));
		splittedNode->m_Next = best->m_Next;
		best->m_Next = splittedNode;
	}
	else
	{
		padding += best->m_Value - (size + padding);
	}

	if (prev == nullptr)
	{
		m_List.m_Head = best->m_Next;
	}
	else
	{
		prev->m_Next = best->m_Next;
	}

	Header* header = reinterpret_cast<Header*>(best);
	header->m_Size = size + padding;

	return reinterpret_cast<char*>(best) + sizeof(Header);
}

void A5::FreeListAllocator::Deallocate(void* ptr)
{
	Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));

	LinkedList::Node* node = reinterpret_cast<LinkedList::Node*>(header);
	node->m_Value = header->m_Size;

	LinkedList::Node* prevIt = nullptr;
	LinkedList::Node* it = m_List.m_Head;
	while (it != nullptr)
	{
		if (node < it)
		{
			node->m_Next = it;
			if (prevIt == nullptr)
			{
				m_List.m_Head = node;
			}
			else
			{
				prevIt->m_Next = node;
			}
			break;
		}
		prevIt = it;
		it = it->m_Next;
	}

	Coalescence(prevIt, node);
}

void A5::FreeListAllocator::Reset()
{
	Init();
}

void A5::FreeListAllocator::Init()
{
	LinkedList::Node* head = reinterpret_cast<LinkedList::Node*>(m_StartAddress);
	head->m_Value = m_Size - sizeof(Header);
	head->m_Next = nullptr;
	m_List.m_Head = head;
}

void A5::FreeListAllocator::Coalescence(LinkedList::Node* prev, LinkedList::Node* curr)
{
	if (curr->m_Next != nullptr && (std::size_t)curr + curr->m_Value + sizeof(Header) == (std::size_t)curr->m_Next)
	{
		curr->m_Value += curr->m_Next->m_Value + sizeof(Header);
		curr->m_Next = curr->m_Next->m_Next;
	}

	if (prev != nullptr && (std::size_t)prev + prev->m_Value + sizeof(Header) == (std::size_t)curr)
	{
		prev->m_Value += curr->m_Value + sizeof(Header);
		prev->m_Next = curr->m_Next;
	}
}
