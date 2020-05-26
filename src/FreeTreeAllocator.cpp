#include "A5/FreeTreeAllocator.h"

#include <memory>
#include <algorithm>
#include <cassert>

A5::FreeTreeAllocator::FreeTreeAllocator(const std::size_t size)
	: Allocator(size)
{
	assert(size > sizeof(RBTree::Node) * 2 && "Total size must be bigger than size of RBTree::Node * 2 for allocator with atleast 0 byte space");
	m_StartAddress = ::operator new(size);
	Init();
}

A5::FreeTreeAllocator::~FreeTreeAllocator()
{
	::operator delete(m_StartAddress);
	m_StartAddress = nullptr;
}

void* A5::FreeTreeAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	std::size_t padding = size + sizeof(Header) < sizeof(RBTree::Node) ? sizeof(RBTree::Node) - sizeof(Header) - size : 0;
	void* currentAddress = (void*)(sizeof(Header) + size + padding);
	void* nextAddress = (void*)(sizeof(Header) + size + padding);
	std::size_t space = size + padding + 100;
	std::align(alignof(std::max_align_t), sizeof(std::max_align_t), nextAddress, space);
	padding += (std::size_t)nextAddress - (std::size_t)currentAddress;

	RBTree::Node* node = m_Tree.SearchBest(size + padding);

	if (node == nullptr || node->m_Value < size + padding)
	{
		return nullptr;
	}

	m_Tree.Remove(node);

	if (node->m_Value >= size + padding + sizeof(RBTree::Node))
	{
		RBTree::Node* splittedNode = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(node) + sizeof(Header) + size + padding);
		splittedNode->m_Value = node->m_Value - (size + padding + sizeof(Header));
		m_Tree.Insert(splittedNode);
		std::size_t* nextBlockAddress = reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(splittedNode) + sizeof(Header) + splittedNode->m_Value);
		if ((std::size_t)nextBlockAddress <= (std::size_t)m_StartAddress + m_Size - sizeof(std::size_t))
		{
			*nextBlockAddress = sizeof(Header) + splittedNode->m_Value;
		}
	}
	else
	{
		padding += node->m_Value - (size + padding);
	}

	Header* header = reinterpret_cast<Header*>(node);
	header->m_Size = size + padding;

	*reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(header) + sizeof(Header) + header->m_Size) = 0;

	return reinterpret_cast<char*>(node) + sizeof(Header);
}

void A5::FreeTreeAllocator::Deallocate(void* ptr)
{
	Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));
	*reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(header) + sizeof(Header) + header->m_Size) = sizeof(Header) + header->m_Size;
	RBTree::Node* node = reinterpret_cast<RBTree::Node*>(header);
	node->m_Value = header->m_Size;
	Coalescence(node);
}

void A5::FreeTreeAllocator::Reset()
{
	Init();
}

void A5::FreeTreeAllocator::Init()
{
	RBTree::Node* nil = reinterpret_cast<RBTree::Node*>(m_StartAddress);
	m_Tree.Init(nil);
	void* currentAddress = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(m_StartAddress) + sizeof(RBTree::Node) + sizeof(Header));
	std::size_t space = m_Size - sizeof(Header) - sizeof(RBTree::Node);
	std::align(alignof(std::max_align_t), sizeof(std::max_align_t), currentAddress, space);
	RBTree::Node* root = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(currentAddress) - sizeof(Header));
	root->m_Value = reinterpret_cast<char*>(m_StartAddress) + m_Size - reinterpret_cast<char*>(root) - sizeof(Header);
	m_Tree.Insert(root);
}

void A5::FreeTreeAllocator::Coalescence(RBTree::Node* curr)
{
	RBTree::Node* next = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(curr) + sizeof(Header) + curr->m_Value);
	if (((std::size_t)next < (std::size_t)m_StartAddress + m_Size) && (std::size_t)next->GetParentRaw() & 1)
	{
		curr->m_Value += next->m_Value + sizeof(Header);
		m_Tree.Remove(next);
	}

	if (curr->m_PrevSize != 0)
	{
		RBTree::Node* prev = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(curr) - curr->m_PrevSize);
		m_Tree.Remove(prev);
		prev->m_Value += curr->m_Value + sizeof(Header);
		m_Tree.Insert(prev);
	}
	else
	{
		m_Tree.Insert(curr);
	}
}
