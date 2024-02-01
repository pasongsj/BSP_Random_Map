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

class Node : public std::enable_shared_from_this<Node>
{
public:
	std::weak_ptr<Node> parNode;			// �θ���
	std::shared_ptr<Node> leftNode;			// ���� �ڽ� ���
	std::shared_ptr<Node> rightNode;		// ������ �ڽ� ���
	RectInt nodeRect;		// ���� ����

	//������
	Node(RectInt rect)
	{
		nodeRect = rect;
		parNode;
		leftNode = nullptr;
		rightNode = nullptr;
	}

	Node()
	{
		nodeRect = { 0,0,0,0 };
		parNode;
		leftNode = nullptr;
		rightNode = nullptr;
	}

	~Node()
	{
		parNode;
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
		if (!parNode.expired())
		{
			return nullptr;
		}

		// ������ ���鼭
		if (parNode.lock()->rightNode == shared_from_this())
		{
			if(nullptr != parNode.lock()->leftNode) // ���ʳ�尡 �����Ѵٸ�
			{
				return parNode.lock()->leftNode->MaxNode(); // ���ʳ���� max��
			}
			return parNode.lock()->GetBeforeNode();
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
		if (!parNode.expired())
		{
			return nullptr;
		}
		if (parNode.lock()->leftNode == shared_from_this())
		{
			return parNode.lock()->GetParentNode_RIghtChild();
		}
		return parNode.lock();
	}
};