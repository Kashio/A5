#include "A5/RBTree.h"

A5::RBTree::RBTree()
{
}

void A5::RBTree::Init(Node* nil)
{
	nil->SetColor(NodeColor::BLACK);
	m_Nil = nil;
	m_Nil->m_Left = nil;
	m_Nil->m_Right = nil;
	m_Root = nil;
}

A5::RBTree::Node* A5::RBTree::Search(const int v)
{
	Node* x = m_Root;
	while (x != m_Nil)
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

A5::RBTree::Node* A5::RBTree::SearchBest(const int v)
{
	Node* y = nullptr;
	Node* x = m_Root;
	while (x != m_Nil)
	{
		y = x;
		if (v == x->m_Value)
			break;
		else if (v < x->m_Value)
			x = x->m_Left;
		else
			x = x->m_Right;
	}
	return y;
}

void A5::RBTree::Insert(Node* z)
{
	z->m_Left = m_Nil;
	z->m_Right = m_Nil;
	z->SetColor(NodeColor::RED);

	Node* y = nullptr;
	Node* x = m_Root;
	while (x != m_Nil)
	{
		y = x;
		if (z->m_Value < x->m_Value)
			x = x->m_Left;
		else
			x = x->m_Right;
	}

	z->SetParent(y);
	if (y == nullptr)
		m_Root = z;
	else if (z->m_Value < y->m_Value)
		y->m_Left = z;
	else
		y->m_Right = z;

	InsertFixup(z);
}

void A5::RBTree::InsertFixup(Node* z)
{
	while (z != m_Root && z->GetParent()->GetColor() == NodeColor::RED)
	{
		if (z->GetParent() == z->GetParent()->GetParent()->m_Left)
		{
			Node* y = z->GetParent()->GetParent()->m_Right;
			if (y->GetColor() == NodeColor::RED)
			{
				z->GetParent()->SetColor(NodeColor::BLACK);
				y->SetColor(NodeColor::BLACK);
				z->GetParent()->GetParent()->SetColor(NodeColor::RED);
				z = z->GetParent()->GetParent();
			}
			else if (z == z->GetParent()->m_Right)
			{
				z = z->GetParent();
				LeftRotate(z);
			}
			else
			{
				z->GetParent()->SetColor(NodeColor::BLACK);
				z->GetParent()->GetParent()->SetColor(NodeColor::RED);
				RightRotate(z->GetParent()->GetParent());
			}
		}
		else
		{
			Node* y = z->GetParent()->GetParent()->m_Left;
			if (y->GetColor() == NodeColor::RED)
			{
				z->GetParent()->SetColor(NodeColor::BLACK);
				y->SetColor(NodeColor::BLACK);
				z->GetParent()->GetParent()->SetColor(NodeColor::RED);
				z = z->GetParent()->GetParent();
			}
			else if (z == z->GetParent()->m_Left)
			{
				z = z->GetParent();
				RightRotate(z);
			}
			else
			{
				z->GetParent()->SetColor(NodeColor::BLACK);
				z->GetParent()->GetParent()->SetColor(NodeColor::RED);
				LeftRotate(z->GetParent()->GetParent());
			}
		}
	}
	m_Root->SetColor(NodeColor::BLACK);
	m_Root->m_PrevSize = 0;
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

	if (z->GetColor() == NodeColor::RED)
	{
		if (x == m_Nil || x->GetColor() == NodeColor::BLACK)
		{
			x->SetColor(NodeColor::RED);
			RemoveFixup(y);
		}
	}
	else
	{
		if (x == m_Nil || x->GetColor() == NodeColor::BLACK)
		{
			RemoveFixup(y);
		}
		else
		{
			x->SetColor(NodeColor::BLACK);
		}
	}

	//m_NodePool.Deallocate(z);
}

void A5::RBTree::RemoveFixup(Node* z)
{
	while (z != m_Root && z->GetColor() == NodeColor::BLACK)
	{
		Node* w = nullptr;
		if (z->GetParent()->m_Left == z)
		{
			w = z->GetParent()->m_Right;
			if (w->GetColor() == NodeColor::RED)
			{
				w->SetColor(NodeColor::BLACK);
				z->GetParent()->SetColor(NodeColor::RED);
				LeftRotate(z->GetParent());
			}
			else
			{
				if ((w->m_Right == m_Nil || w->m_Right->GetColor() == NodeColor::BLACK) && (w->m_Left == m_Nil || w->m_Left->GetColor() == NodeColor::BLACK))
				{
					w->SetColor(NodeColor::RED);
					z = z->GetParent();
				}
				else if (w->m_Left != m_Nil && w->m_Left->GetColor() == NodeColor::RED && (w->m_Right == m_Nil || w->m_Right->GetColor() == NodeColor::BLACK))
				{
					w->m_Left->SetColor(NodeColor::BLACK);
					w->SetColor(NodeColor::RED);
					RightRotate(w);
				}
				else if (w->m_Right != m_Nil && w->m_Right->GetColor() == NodeColor::RED)
				{
					w->SetColor(z->GetParent()->GetColor());
					z->GetParent()->SetColor(NodeColor::BLACK);
					w->m_Right->SetColor(NodeColor::BLACK);
					LeftRotate(z->GetParent());
					break;
				}
			}
		}
		else
		{
			w = z->GetParent()->m_Left;
			if (w->GetColor() == NodeColor::RED)
			{
				w->SetColor(NodeColor::BLACK);
				z->GetParent()->SetColor(NodeColor::RED);
				RightRotate(z->GetParent());
			}
			else
			{
				if ((w->m_Right == m_Nil || w->m_Right->GetColor() == NodeColor::BLACK) && (w->m_Left == m_Nil || w->m_Left->GetColor() == NodeColor::BLACK))
				{
					w->SetColor(NodeColor::RED);
					z = z->GetParent();
				}
				else if (w->m_Right != m_Nil && w->m_Right->GetColor() == NodeColor::RED && (w->m_Left == m_Nil || w->m_Left->GetColor() == NodeColor::BLACK))
				{
					w->m_Right->SetColor(NodeColor::BLACK);
					w->SetColor(NodeColor::RED);
					LeftRotate(w);
				}
				else if (w->m_Left != m_Nil && w->m_Left->GetColor() == NodeColor::RED)
				{
					w->SetColor(z->GetParent()->GetColor());
					z->GetParent()->SetColor(NodeColor::BLACK);
					w->m_Left->SetColor(NodeColor::BLACK);
					RightRotate(z->GetParent());
					break;
				}
			}
		}
	}
	z->SetColor(NodeColor::BLACK);
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
	v->SetParent(u->GetParent());
	if (u->m_Left != v)
	{
		v->m_Left = u->m_Left;
		if (u->m_Left != m_Nil)
			u->m_Left->SetParent(v);
	}
	if (u->m_Right != v)
	{
		v->m_Right = u->m_Right;
		if (u->m_Right != m_Nil)
			u->m_Right->SetParent(v);
	}
	if (u->GetParent() == nullptr)
		m_Root = v;
	else if (u == u->GetParent()->m_Left)
		u->GetParent()->m_Left = v;
	else
		u->GetParent()->m_Right = v;
}

void A5::RBTree::LeftRotate(Node* x)
{
	Node* y = x->m_Right;
	x->m_Right = y->m_Left;
	if (y->m_Left != m_Nil)
		y->m_Left->SetParent(x);
	y->SetParent(x->GetParent());
	if (x->GetParent() == nullptr)
		m_Root = y;
	else if (x->GetParent()->m_Left == x)
		x->GetParent()->m_Left = y;
	else
		x->GetParent()->m_Right = y;
	y->m_Left = x;
	x->SetParent(y);
}

void A5::RBTree::RightRotate(Node* x)
{
	Node* y = x->m_Left;
	x->m_Left = y->m_Right;
	if (y->m_Right != m_Nil)
		y->m_Right->SetParent(x);
	y->SetParent(x->GetParent());
	if (x->GetParent() == nullptr)
		m_Root = y;
	else if (x->GetParent()->m_Left == x)
		x->GetParent()->m_Left = y;
	else
		x->GetParent()->m_Right = y;
	y->m_Right = x;
	x->SetParent(y);
}
