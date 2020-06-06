#include "A5/RBTree.h"

#include <iostream>

void A5::RBTree::Init(Node* nil)
{
	m_Nil = nil;
	m_Nil->SetColor(NodeColor::BLACK);
	m_Nil->m_Left = m_Nil;
	m_Nil->m_Right = m_Nil;
	m_Nil->m_Value = 0;
	m_Root = m_Nil;
}

A5::RBTree::Node* A5::RBTree::Search(const std::size_t v)
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

A5::RBTree::Node* A5::RBTree::SearchBest(const std::size_t v)
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
	while (y != nullptr && v > y->m_Value)
		y = y->GetParent();
	return y;
}

A5::RBTree::Node* A5::RBTree::SearchAtLeast(const std::size_t v)
{
	Node* x = m_Root;
	while (x != m_Nil)
	{
		if (v <= x->m_Value)
			return x;
		else
			x = x->m_Right;
	}
	return nullptr;
}

void A5::RBTree::Insert(Node* z)
{
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

	z->m_Left = m_Nil;
	z->m_Right = m_Nil;
	z->SetColor(NodeColor::RED);

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
			else
			{
				if (z == z->GetParent()->m_Right)
				{
					z = z->GetParent();
					LeftRotate(z);
				}
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
			else
			{
				if (z == z->GetParent()->m_Left)
				{
					z = z->GetParent();
					RightRotate(z);
				}
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
	Node* y = z;
	NodeColor yOriginalColor = y->GetColor();
	if (z->m_Left == m_Nil)
	{
		x = z->m_Right;
		Transplant(z, z->m_Right);
	}
	else if (z->m_Right == m_Nil)
	{
		x = z->m_Left;
		Transplant(z, z->m_Left);
	}
	else
	{
		y = Successor(z);
		yOriginalColor = y->GetColor();
		x = y->m_Right;
		if (y->GetParent() == z)
		{
			x->SetParent(y);
		}
		else
		{
			Transplant(y, y->m_Right);
			y->m_Right = z->m_Right;
			if (y->m_Right != m_Nil)
				y->m_Right->SetParent(y);
		}
		Transplant(z, y);
		y->m_Left = z->m_Left;
		if (y->m_Left != m_Nil)
			y->m_Left->SetParent(y);
		y->SetColor(z->GetColor());
	}

	if (yOriginalColor == NodeColor::BLACK)
		RemoveFixup(x);
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
				w = z->GetParent()->m_Right;
			}
			if ((w->m_Right == m_Nil || w->m_Right->GetColor() == NodeColor::BLACK) && (w->m_Left == m_Nil || w->m_Left->GetColor() == NodeColor::BLACK))
			{
				w->SetColor(NodeColor::RED);
				z = z->GetParent();
			}
			else
			{
				if (w->m_Right == m_Nil || w->m_Right->GetColor() == NodeColor::BLACK)
				{
					w->m_Left->SetColor(NodeColor::BLACK);
					w->SetColor(NodeColor::RED);
					RightRotate(w);
					w = z->GetParent()->m_Right;
				}
				w->SetColor(z->GetParent()->GetColor());
				z->GetParent()->SetColor(NodeColor::BLACK);
				w->m_Right->SetColor(NodeColor::BLACK);
				LeftRotate(z->GetParent());
				z = m_Root;
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
				w = z->GetParent()->m_Left;
			}
			if ((w->m_Right == m_Nil || w->m_Right->GetColor() == NodeColor::BLACK) && (w->m_Left == m_Nil || w->m_Left->GetColor() == NodeColor::BLACK))
			{
				w->SetColor(NodeColor::RED);
				z = z->GetParent();
			}
			else
			{
				if (w->m_Left == m_Nil || w->m_Left->GetColor() == NodeColor::BLACK)
				{
					w->m_Right->SetColor(NodeColor::BLACK);
					w->SetColor(NodeColor::RED);
					LeftRotate(w);
					w = z->GetParent()->m_Left;
				}
				w->SetColor(z->GetParent()->GetColor());
				z->GetParent()->SetColor(NodeColor::BLACK);
				w->m_Left->SetColor(NodeColor::BLACK);
				RightRotate(z->GetParent());
				z = m_Root;
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

void A5::RBTree::Print() const
{
	if (m_Root->m_Right != m_Nil) {
		Print(m_Root->m_Right, true, "");
	}
	std::cout << m_Root->m_Value << '\n';
	if (m_Root->m_Left != m_Nil) {
		Print(m_Root->m_Left, false, "");
	}
}

void A5::RBTree::Print(Node* x, bool isRight, std::string indent) const
{
	if (x->m_Right != m_Nil) {
		Print(x->m_Right, true, indent + (isRight ? "        " : " |      "));
	}
	std::cout << indent;
	if (isRight) {
		std::cout << " /";
	}
	else {
		std::cout << " \\";
	}
	std::cout << "----- ";
	std::cout << x->m_Value << '\n';
	if (x->m_Left != m_Nil) {
		Print(x->m_Left, false, indent + (isRight ? " |      " : "        "));
	}
}

void A5::RBTree::Transplant(Node* u, Node* v)
{
	Node* uParent = u->GetParent();
	if (uParent == nullptr)
		m_Root = v;
	else if (u == uParent->m_Left)
		uParent->m_Left = v;
	else
		uParent->m_Right = v;
	v->SetParent(uParent);
}

void A5::RBTree::LeftRotate(Node* x)
{
	Node* xParent = x->GetParent();
	Node* y = x->m_Right;
	x->m_Right = y->m_Left;
	if (y->m_Left != m_Nil)
		y->m_Left->SetParent(x);
	y->SetParent(xParent);
	if (xParent == nullptr)
		m_Root = y;
	else if (xParent->m_Left == x)
		xParent->m_Left = y;
	else
		xParent->m_Right = y;
	y->m_Left = x;
	x->SetParent(y);
}

void A5::RBTree::RightRotate(Node* x)
{
	Node* xParent = x->GetParent();
	Node* y = x->m_Left;
	x->m_Left = y->m_Right;
	if (y->m_Right != m_Nil)
		y->m_Right->SetParent(x);
	y->SetParent(xParent);
	if (xParent == nullptr)
		m_Root = y;
	else if (xParent->m_Left == x)
		xParent->m_Left = y;
	else
		xParent->m_Right = y;
	y->m_Right = x;
	x->SetParent(y);
}
