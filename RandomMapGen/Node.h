#pragma once

#include <memory>
#include <iostream>
class RectInt // 공간의 정보
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
	int x;		// 시작점x
	int y;		// 시작점 y
	int height; // x의 길이
	int width;  // y의 길이
};

class Node : public std::enable_shared_from_this<Node>
{
public:
	std::shared_ptr<Node> parNode;			// 부모노드
	std::shared_ptr<Node> leftNode;			// 왼쪽 자식 노드
	std::shared_ptr<Node> rightNode;		// 오른쪽 자식 노드
	RectInt nodeRect;		// 공간 정보

	//생성자
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

	// 현재 노드 기준으로 왼쪽 끝
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

	// 현재 노드 기준으로 오른쪽 끝
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

	// 이전 리프노드
	std::shared_ptr<Node> GetBeforeNode()
	{
		if (nullptr == parNode)
		{
			return nullptr;
		}

		// 오른쪽 노드면서
		if (parNode->rightNode == shared_from_this())
		{
			if(nullptr != parNode->leftNode) // 왼쪽노드가 존재한다면
			{
				return parNode->leftNode->MaxNode(); // 왼쪽노드의 max값
			}
			return parNode->GetBeforeNode();
		}
		else // 왼쪽 노드라면
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

	// 이전 리프노드
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