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
	int x;		// ������x
	int y;		// ������ y
	int height; // x�� ����
	int width;  // y�� ����
};

class Node : public std::enable_shared_from_this<Node>
{
public:
	std::shared_ptr<Node> parNode;			// �θ���
	std::shared_ptr<Node> leftNode;			// ���� �ڽ� ���
	std::shared_ptr<Node> rightNode;		// ������ �ڽ� ���
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
	std::shared_ptr<Node> MinNode()
	{
		if (nullptr != leftNode)
		{
			return leftNode->MinNode();
		}
		if (nullptr != rightNode)
		{
			return rightNode->MaxNode();
		}
		return shared_from_this();
	}

	// ���� ��� �������� ������ ��
	std::shared_ptr<Node> MaxNode()
	{
		if (nullptr != rightNode)
		{
			return rightNode->MaxNode();
		}
		if (nullptr != leftNode)
		{
			return leftNode->MaxNode();
		}
		return shared_from_this();
	}

	// ���� �������
	std::shared_ptr<Node> GetBeforeNode()
	{
		if (nullptr == parNode)
		{
			return nullptr;
		}

		// ������ ���鼭
		if (parNode->rightNode == shared_from_this())
		{
			if(nullptr != parNode->leftNode) // ���ʳ�尡 �����Ѵٸ�
			{
				return parNode->leftNode->MaxNode(); // ���ʳ���� max��
			}
			return parNode->GetBeforeNode();
		}
		else // ���� �����
		{
			std::shared_ptr<Node> lastpar = GetParentNode_RIghtChild();
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
	std::shared_ptr<Node> GetParentNode_RIghtChild()
	{
		if (nullptr == parNode)
		{
			return nullptr;
		}
		if (parNode->leftNode == shared_from_this())
		{
			return parNode->GetParentNode_RIghtChild();
		}
		return parNode;
	}
};