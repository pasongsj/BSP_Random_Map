#pragma once

#include <memory>
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
};