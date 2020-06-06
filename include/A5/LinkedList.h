#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstddef>

namespace A5
{
	class LinkedList
	{
	public:
		struct Node
		{
			Node* m_Next;
			std::size_t m_Value;
		};
		Node* m_Head;
	public:
		void SearchFirst(const std::size_t v, Node*& curr, Node*& prev);
		void SearchBest(const std::size_t v, Node*& curr, Node*& prev);
	};
};

#endif // !LINKED_LIST_H