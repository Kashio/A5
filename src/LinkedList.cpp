#include "A5/LinkedList.h"

void A5::LinkedList::SearchFirst(const std::size_t v, Node*& curr, Node*& prev)
{
	Node* prevX = nullptr;
	Node* x = m_Head;
	while (x != nullptr)
	{
		if (v <= x->m_Value)
			break;
		prevX = x;
		x = x->m_Next;
	}
	prev = prevX;
	curr = x;
}

void A5::LinkedList::SearchBest(const std::size_t v, Node*& curr, Node*& prev)
{
	Node* prevBest = nullptr;
	Node* best = nullptr;
	Node* prevX = nullptr;
	Node* x = m_Head;
	while (x != nullptr)
	{
		if (v == x->m_Value)
		{
			prevBest = prevX;
			best = x;
			break;
		}
		else if (x->m_Value > v)
		{
			if (best == nullptr || x->m_Value < best->m_Value)
			{
				prevBest = prevX;
				best = x;
			}
		}
		prevX = x;
		x = x->m_Next;
	}
	prev = prevBest;
	curr = best;
}
