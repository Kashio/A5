#include "A5/BuddyAllocator.h"

#include <memory>
#include <cmath>

A5::BuddyAllocator::BuddyAllocator(const std::size_t size)
	: Allocator(size)
{
	m_StartAddress = ::operator new(size);
	Init();
}

A5::BuddyAllocator::~BuddyAllocator()
{
	::operator delete(m_StartAddress);
	m_StartAddress = nullptr;
}

void* A5::BuddyAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	int bucket = std::max(int(std::ceil(std::log2(size + sizeof(Header))) - 1 - s_Log2Header), 0);

	if (m_Buckets[bucket] != nullptr)
	{
		Node* node = m_Buckets[bucket];
		m_Buckets[bucket] = node->m_Next;
		Header* header = reinterpret_cast<Header*>(node);
		header->m_Size = (std::size_t)std::pow(2, bucket + 1 + s_Log2Header) | 1;
		void* address = (void*)(reinterpret_cast<char*>(node) + sizeof(Header));
		return address;
	}

	int i;

	for (i = bucket + 1; i < m_Buckets.size(); ++i)
	{
		if (m_Buckets[i] != nullptr)
			break;
	}

	if (i == m_Buckets.size())
		return nullptr;

	Node* temp = m_Buckets[i];
	m_Buckets[i] = temp->m_Next;

	--i;

	for (; i >= bucket; i--)
	{
		Node* node = reinterpret_cast<Node*>(reinterpret_cast<char*>(temp) + (std::size_t)std::pow(2, i + 1 + s_Log2Header));
		node->m_Next = m_Buckets[i];
		m_Buckets[i] = node;
	}

	Header* header = reinterpret_cast<Header*>(temp);
	header->m_Size = (std::size_t)std::pow(2, i + 2 + s_Log2Header) | 1;
	void* address = (void*)(reinterpret_cast<char*>(temp) + sizeof(Header));

	return address;
}

void A5::BuddyAllocator::Deallocate(void* ptr)
{
	Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));

	const std::size_t size = header->m_Size & ~(std::size_t)1;
	const std::size_t bucket = (std::size_t)std::log2(size) - 1 - s_Log2Header;

	Node* node = reinterpret_cast<Node*>(header);

	std::size_t buddyNumber;
	char* buddyAddress;

	buddyNumber = (reinterpret_cast<char*>(header) - static_cast<char*>(m_StartAddress)) / size;

	if (buddyNumber % 2 == 0)
		buddyAddress = reinterpret_cast<char*>(header) + size;
	else
		buddyAddress = reinterpret_cast<char*>(header) - size;

	// Check if buddy is occupied to bale early from searching for it
	if (buddyAddress == (static_cast<char*>(m_StartAddress) + m_Size) || *reinterpret_cast<std::size_t*>(buddyAddress) & 1)
	{
		node->m_Next = m_Buckets[bucket];
		m_Buckets[bucket] = node;
	}
	else
	{
		Node* prevBuddy = nullptr;
		Node* buddy = reinterpret_cast<Node*>(buddyAddress);
		Node* context = m_Buckets[bucket];

		// Search the bucket for the buddy and update linked listed
		// This could be improved from O(N) to O(LogN) with RBTree of addresses of nodes
		while (context != buddy && context != nullptr)
		{
			prevBuddy = context;
			context = context->m_Next;
		}

		// If buddy was not found in the bucket it was probably split so we can't merge
		if (context == nullptr)
		{
			node->m_Next = m_Buckets[bucket];
			m_Buckets[bucket] = node;
		}
		else
		{
			if (prevBuddy == nullptr)
			{
				m_Buckets[bucket] = buddy->m_Next;
			}
			else
			{
				prevBuddy->m_Next = buddy->m_Next;
			}

			if (buddyNumber % 2 == 0)
			{
				node->m_Next = m_Buckets[bucket + 1];
				m_Buckets[bucket + 1] = node;
			}
			else
			{
				buddy->m_Next = m_Buckets[bucket + 1];
				m_Buckets[bucket + 1] = buddy;
			}
		}
	}
}

void A5::BuddyAllocator::Reset()
{
	const std::size_t bucket = (std::size_t)std::ceil(std::log2(m_Size)) - 1 - s_Log2Header;
	for (std::size_t i = 0; i < bucket; ++i)
	{
		m_Buckets[i] = nullptr;
	}
	Init();
}

void A5::BuddyAllocator::Init()
{
	Node* root = reinterpret_cast<Node*>(m_StartAddress);
	root->m_Next = nullptr;
	const std::size_t bucket = (std::size_t)std::ceil(std::log2(m_Size)) - 1 - s_Log2Header;
	m_Buckets[bucket] = root;
}
