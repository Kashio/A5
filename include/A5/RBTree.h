#ifndef RBTREE_H
#define RBTREE_H

#include <string>

namespace A5
{
	class RBTree
	{
	public:
		enum class NodeColor : std::size_t
		{
			BLACK,
			RED
		};
		class Node
		{
		public:
			std::size_t m_PrevSize;
		private:
			Node* m_Parent;
		public:
			Node* m_Right;
			Node* m_Left;
			std::size_t m_Value;

			inline Node* GetParentRaw()
			{
				return m_Parent;
			}

			inline Node* GetParent()
			{
				return reinterpret_cast<Node*>((std::size_t)m_Parent >> 2 << 2);
			}

			inline void SetParent(Node* p)
			{
				m_Parent = reinterpret_cast<Node*>((std::size_t)p | ((std::size_t)m_Parent & 2) |  1);
			}

			inline NodeColor GetColor()
			{
				if (m_Parent == nullptr)
					return NodeColor::BLACK;
				return (std::size_t)m_Parent & 2 ? NodeColor::RED : NodeColor::BLACK;
			}

			inline void SetColor(NodeColor color)
			{
				if (color == NodeColor::RED)
					m_Parent = reinterpret_cast<Node*>((std::size_t)m_Parent | 2);
				else
					m_Parent = reinterpret_cast<Node*>((std::size_t)m_Parent & ~((std::size_t)2));
			}
		};
	private:
		Node* m_Nil;
		Node* m_Root;
	public:
		void Init(Node* nil);
		Node* Search(const std::size_t v);
		Node* SearchBest(const std::size_t v);
		Node* SearchAtLeast(const std::size_t v);
		void Insert(Node* z);
		void Remove(Node* z);
		Node* Successor(Node* x);
		void Print() const;
	private:
		void InsertFixup(Node* z);
		void RemoveFixup(Node* z);
		void Transplant(Node* u, Node* v);
		void LeftRotate(Node* x);
		void RightRotate(Node* x);
		void Print(Node* x, bool isRight, std::string indent) const;
	};
};

#endif // !RBTREE_H