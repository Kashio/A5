#include "A5/RBTree.h"

A5::RBTree::RBTree(const std::size_t poolSize)
	: m_NodePool{ poolSize, sizeof(Node), true }
{
	m_Nil = new(m_NodePool.Allocate(sizeof(Node), alignof(Node))) Node();
	m_Root = m_Nil;
}

A5::RBTree::Node* A5::RBTree::Search(const int v)
{
	Node* x = m_Root;
	while (x != nullptr)
	{
		if (v == x->m_Value)
			break;
		else if (v < x->m_Value)
			x = x->m_Left;
		else
			x = x->m_Right;
	}
	return x;
}

void A5::RBTree::Insert(const int v)
{
	Node* z = new(m_NodePool.Allocate(sizeof(Node), alignof(Node))) Node();
	z->m_Value = v;
	z->m_Left = m_Nil;
	z->m_Right = m_Nil;
	z->m_Color = NodeColor::RED;

	Node* y = nullptr;
	Node* x = m_Root;
	while (x != m_Nil)
	{
		y = x;
		if (v < x->m_Value)
			x = x->m_Left;
		else
			x = x->m_Right;
	}

	z->m_Parent = y;
	if (y == nullptr)
		m_Root = z;
	else if (v < y->m_Value)
		y->m_Left = z;
	else
		y->m_Right = z;

	InsertFixup(z);
}

void A5::RBTree::InsertFixup(Node* z)
{
	while (z != m_Root && z->m_Parent->m_Color == NodeColor::RED)
	{
		if (z->m_Parent == z->m_Parent->m_Parent->m_Left)
		{
			Node* y = z->m_Parent->m_Parent->m_Right;
			if (y->m_Color == NodeColor::RED)
			{
				z->m_Parent->m_Color = NodeColor::BLACK;
				y->m_Color = NodeColor::BLACK;
				z->m_Parent->m_Parent->m_Color = NodeColor::RED;
				z = z->m_Parent->m_Parent;
			}
			else if (z == z->m_Parent->m_Right)
			{
				z = z->m_Parent;
				LeftRotate(z);
			}
			else
			{
				z->m_Parent->m_Color = NodeColor::BLACK;
				z->m_Parent->m_Parent->m_Color = NodeColor::RED;
				RightRotate(z->m_Parent->m_Parent);
			}
		}
		else
		{
			Node* y = z->m_Parent->m_Parent->m_Left;
			if (y->m_Color == NodeColor::RED)
			{
				z->m_Parent->m_Color = NodeColor::BLACK;
				y->m_Color = NodeColor::BLACK;
				z->m_Parent->m_Parent->m_Color = NodeColor::RED;
				z = z->m_Parent->m_Parent;
			}
			else if (z == z->m_Parent->m_Left)
			{
				z = z->m_Parent;
				RightRotate(z);
			}
			else
			{
				z->m_Parent->m_Color = NodeColor::BLACK;
				z->m_Parent->m_Parent->m_Color = NodeColor::RED;
				LeftRotate(z->m_Parent->m_Parent);
			}
		}
	}
	m_Root->m_Color = NodeColor::BLACK;
}

void A5::RBTree::Remove(Node* z)
{
	Node* x = nullptr;
	Node* y = nullptr;
	if (z->m_Left == m_Nil)
	{
		x = z->m_Right;
		y = x;
		Transplant(z, x);
	}
	else if (z->m_Right == m_Nil)
	{
		x = z->m_Left;
		y = x;
		Transplant(z, x);
	}
	else
	{
		x = Successor(z);
		y = x->m_Right;
		Transplant(x, y);
		Transplant(z, x);
	}

	if (z->m_Color == NodeColor::RED)
	{
		if (x != m_Nil && x->m_Color == NodeColor::RED)
		{
			return;
		}
		else
		{
			x->m_Color = NodeColor::RED;
			RemoveFixup(y);
		}
	}
	else
	{
		if (x == m_Nil || x->m_Color == NodeColor::BLACK)
		{
			RemoveFixup(y);
		}
		else
		{
			x->m_Color = NodeColor::BLACK;
		}
	}

	m_NodePool.Deallocate(z);
}

void A5::RBTree::RemoveFixup(Node* z)
{

	while (z != m_Root && z->m_Color == NodeColor::BLACK)
	{
		Node* w = nullptr;
		if (z->m_Parent->m_Left == z)
		{
			w = z->m_Parent->m_Right;
			if (w->m_Color == NodeColor::RED)
			{
				w->m_Color = NodeColor::BLACK;
				z->m_Parent->m_Color = NodeColor::RED;
				LeftRotate(z->m_Parent);
			}
			else
			{
				if ((w->m_Right == m_Nil || w->m_Right->m_Color == NodeColor::BLACK) && (w->m_Left == m_Nil || w->m_Left->m_Color == NodeColor::BLACK))
				{
					w->m_Color = NodeColor::RED;
					z = z->m_Parent;
				}
				else if (w->m_Left != m_Nil && w->m_Left->m_Color == NodeColor::RED && (w->m_Right == m_Nil || w->m_Right->m_Color == NodeColor::BLACK))
				{
					w->m_Left->m_Color = NodeColor::BLACK;
					w->m_Color = NodeColor::RED;
					RightRotate(w);
				}
				else if (w->m_Right != m_Nil && w->m_Right->m_Color == NodeColor::RED)
				{
					w->m_Color = z->m_Parent->m_Color;
					z->m_Parent->m_Color = NodeColor::BLACK;
					w->m_Right->m_Color = NodeColor::BLACK;
					LeftRotate(z->m_Parent);
					break;
				}
			}
		}
		else
		{
			w = z->m_Parent->m_Left;
			if (w->m_Color == NodeColor::RED)
			{
				w->m_Color = NodeColor::BLACK;
				z->m_Parent->m_Color = NodeColor::RED;
				RightRotate(z->m_Parent);
			}
			else
			{
				if ((w->m_Right == m_Nil || w->m_Right->m_Color == NodeColor::BLACK) && (w->m_Left == m_Nil || w->m_Left->m_Color == NodeColor::BLACK))
				{
					w->m_Color = NodeColor::RED;
					z = z->m_Parent;
				}
				else if (w->m_Right != m_Nil && w->m_Right->m_Color == NodeColor::RED && (w->m_Left == m_Nil || w->m_Left->m_Color == NodeColor::BLACK))
				{
					w->m_Right->m_Color = NodeColor::BLACK;
					w->m_Color = NodeColor::RED;
					LeftRotate(w);
				}
				else if (w->m_Left != m_Nil && w->m_Left->m_Color == NodeColor::RED)
				{
					w->m_Color = z->m_Parent->m_Color;
					z->m_Parent->m_Color = NodeColor::BLACK;
					w->m_Left->m_Color = NodeColor::BLACK;
					RightRotate(z->m_Parent);
					break;
				}
			}
		}
	}
	z->m_Color = NodeColor::BLACK;
}

A5::RBTree::Node* A5::RBTree::Successor(Node* x)
{
	x = x->m_Right;
	while (x->m_Left != m_Nil)
	{
		x = x->m_Left;
	}
	return x;
}

void A5::RBTree::Transplant(Node* u, Node* v)
{
	v->m_Parent = u->m_Parent;
	v->m_Left = u->m_Left;
	if (u->m_Left != m_Nil)
		u->m_Left->m_Parent = v;
	v->m_Right = u->m_Right;
	if (u->m_Left != m_Nil)
		u->m_Right->m_Parent = v;
	if (u->m_Parent == nullptr)
		m_Root = v;
	else if (u == u->m_Parent->m_Left)
		u->m_Parent->m_Left = v;
	else
		u->m_Parent->m_Right = v;
}

void A5::RBTree::LeftRotate(Node* x)
{
	Node* y = x->m_Right;
	x->m_Right = y->m_Left;
	if (y->m_Left != m_Nil)
		y->m_Left->m_Parent = x;
	y->m_Parent = x->m_Parent;
	if (x->m_Parent == nullptr)
		m_Root = y;
	else if (x->m_Parent->m_Left == x)
		x->m_Parent->m_Left = y;
	else
		x->m_Parent->m_Right = y;
	y->m_Left = x;
	x->m_Parent = y;
}

void A5::RBTree::RightRotate(Node* x)
{
	Node* y = x->m_Left;
	x->m_Left = y->m_Right;
	if (y->m_Right != m_Nil)
		y->m_Right->m_Parent = x;
	y->m_Parent = x->m_Parent;
	if (x->m_Parent == nullptr)
		m_Root = y;
	else if (x->m_Parent->m_Left == x)
		x->m_Parent->m_Left = y;
	else
		x->m_Parent->m_Right = y;
	y->m_Right = x;
	x->m_Parent = y;
}
