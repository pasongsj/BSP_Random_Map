#pragma once

#include <memory>
#include <iostream>
class RectInt // ������ ����
{
public:
	RectInt(int _x, int _y, int _h, int _w)
		:x(_x), y(_y), height(_h), width(_w)
	{
	}
	RectInt()
		:x(0), y(0), width(0), height(0)
	{
	}
	int GetMidx() const
	{
		return x + height / 2;
	}

	int GetMidy() const
	{
		return y + width / 2;
	}

	std::pair<int, int> GetMid()
	{
		return std::make_pair(x + height / 2, y + width / 2);
	}
	int x;		// ������x
	int y;		// ������ y
	int height; // x�� ����
	int width;  // y�� ����

};

class Node
{
public:
	Node* parNode;			// �θ���
	Node* leftNode;			// ���� �ڽ� ���
	Node* rightNode;		// ������ �ڽ� ���
	RectInt nodeRect;		// ���� ����

	//������
	Node(RectInt rect)
	{
		nodeRect = rect;
		parNode = nullptr;
		leftNode = nullptr;
		rightNode = nullptr;
	}

	Node()
	{
		nodeRect = { 0,0,0,0 };
		parNode = nullptr;
		leftNode = nullptr;
		rightNode = nullptr;
	}

	~Node()
	{
		parNode = nullptr;
		leftNode = nullptr;
		rightNode = nullptr;
	}

	// for debug
	void PrintNode()
	{
		std::cout << nodeRect.x <<',' << nodeRect.y <<':'<< nodeRect.x + nodeRect.height <<','<< nodeRect.y + nodeRect.width << '\n';
	}

	// ���� ��� �������� ���� ��
	Node* MinNode()
	{
		if (nullptr != leftNode)
		{
			return leftNode->MinNode();
		}
		if (nullptr != rightNode)
		{
			return rightNode->MaxNode();
		}
		return this;
	}

	// ���� ��� �������� ������ ��
	Node* MaxNode()
	{
		if (nullptr != rightNode)
		{
			return rightNode->MaxNode();
		}
		if (nullptr != leftNode)
		{
			return leftNode->MaxNode();
		}
		return this;
	}

	// ���� �������
	Node* GetBeforeNode()
	{
		if (nullptr == parNode)
		{
			return nullptr;
		}

		// ������ ���鼭
		if (parNode->rightNode == this)
		{
			if(nullptr != parNode->leftNode) // ���ʳ�尡 �����Ѵٸ�
			{
				return parNode->leftNode->MaxNode(); // ���ʳ���� max��
			}
			return parNode->GetBeforeNode();
		}
		else // ���� �����
		{
			Node* lastpar = GetParentNode_RIghtChild();
			if (nullptr == lastpar)
			{
				return nullptr;
			}
			if (nullptr == lastpar->leftNode)
			{
				return lastpar->GetBeforeNode();
			}
			return lastpar->leftNode->MaxNode();
		}
	}

	// ���� �������
	Node* GetParentNode_RIghtChild()
	{
		if (nullptr == parNode)
		{
			return nullptr;
		}
		if (parNode->leftNode == this)
		{
			return parNode->GetParentNode_RIghtChild();
		}
		return parNode;
	}
};