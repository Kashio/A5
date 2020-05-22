#ifndef RBTREE_H
#define RBTREE_H

#include "PoolAllocator.h";

namespace A5
{
	class RBTree
	{
	public:
		enum class NodeColor
		{
			BLACK,
			RED
		};
		struct Node
		{
			Node* m_Parent;
			Node* m_Right;
			Node* m_Left;
			int m_Value;
			NodeColor m_Color;
		};
	public:
		Node* m_Nil;
		Node* m_Root;
		A5::PoolAllocator m_NodePool;
	public:
		RBTree(const std::size_t poolSize);
		Node* Search(const int v);
		void Insert(const int v);
		void Remove(Node* z);
		Node* Successor(Node* x);
	private:
		void InsertFixup(Node* z);
		void RemoveFixup(Node* z);
		void Transplant(Node* u, Node* v);
		void LeftRotate(Node* x);
		void RightRotate(Node* x);
	};
};

#endif // !RBTREE_H