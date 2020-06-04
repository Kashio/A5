#include "A5/BuddyAllocator.h"

#include <memory>
#include <cmath>

A5::BuddyAllocator::BuddyAllocator(const std::size_t size)
	: Allocator(size)
{
	m_StartAddress = ::operator new(size);
	std::size_t x = (std::size_t)std::ceil(std::log2(m_Size));
	m_FreeLists.reserve(x + 1);
	Init(x);
}

A5::BuddyAllocator::~BuddyAllocator()
{
	::operator delete(m_StartAddress);
	m_StartAddress = nullptr;
}

void* A5::BuddyAllocator::Allocate(const std::size_t size, const std::size_t alignment)
{
	int x = (int)std::ceil(std::log2(size));

	if (m_FreeLists[x].size() > 0)
	{
		void* address = (void*)m_FreeLists[x][0].m_LowerBound;
		m_BlockSize[m_FreeLists[x][0].m_LowerBound] = m_FreeLists[x][0].m_UpperBound - m_FreeLists[x][0].m_LowerBound + 1;
		m_FreeLists[x].erase(m_FreeLists[x].begin());
		return address;
	}

	int i;

	for (i = x + 1; i < m_FreeLists.size(); i++) {

		if (m_FreeLists[i].size() > 0)
			break;
	}

	if (i == m_FreeLists.size())
	{
		return nullptr;
	}

	Boundry temp = m_FreeLists[i][0];
	m_FreeLists[i].erase(m_FreeLists[i].begin());

	--i;

	for (; i >= x; i--)
	{
		m_FreeLists[i].emplace_back(temp.m_LowerBound + (temp.m_UpperBound - temp.m_LowerBound + 1) / 2, temp.m_UpperBound);
		temp.m_UpperBound = temp.m_LowerBound + (temp.m_UpperBound - temp.m_LowerBound) / 2;
	}

	m_BlockSize[temp.m_LowerBound] = temp.m_UpperBound - temp.m_LowerBound + 1;
	return (void*)temp.m_LowerBound;
}

void A5::BuddyAllocator::Deallocate(void* ptr)
{
	auto it = m_BlockSize.find(static_cast<char*>(ptr));
	if (it == m_BlockSize.end())
		return;

	std::size_t x = (std::size_t)std::log2(it->second);
	std::size_t buddyNumber;
	char* buddyAddress;

	m_FreeLists[x].emplace_back(static_cast<char*>(ptr), static_cast<char*>(ptr) + it->second - 1);

	buddyNumber = (static_cast<char*>(ptr) - static_cast<char*>(m_StartAddress)) / it->second;

	if (buddyNumber % 2 == 0)
		buddyAddress = static_cast<char*>(ptr) + it->second;
	else
		buddyAddress = static_cast<char*>(ptr) - it->second;

	for (std::size_t i = 0; i < m_FreeLists[x].size(); i++)
	{
		if (m_FreeLists[x][i].m_LowerBound == buddyAddress)
		{
			if (buddyNumber % 2 == 0)
				m_FreeLists[x + 1].emplace_back(static_cast<char*>(ptr), m_FreeLists[x][i].m_UpperBound);
			else
				m_FreeLists[x + 1].emplace_back(buddyAddress, static_cast<char*>(ptr) + it->second - 1);

			m_FreeLists[x].erase(m_FreeLists[x].begin() + i);
			m_FreeLists[x].erase(m_FreeLists[x].end() - 1);
			break;
		}
	}

	m_BlockSize.erase(it);
}

void A5::BuddyAllocator::Reset()
{
	std::size_t x = (std::size_t)std::ceil(std::log2(m_Size));
	Init(x);
}

void A5::BuddyAllocator::Init(const std::size_t x)
{
	m_BlockSize.clear();
	m_FreeLists.clear();

	for (std::size_t i = 0; i <= x; i++)
		m_FreeLists.push_back(std::vector<Boundry>());

	m_FreeLists[x].emplace_back(static_cast<char*>(m_StartAddress), static_cast<char*>(m_StartAddress) + m_Size - 1);
}
