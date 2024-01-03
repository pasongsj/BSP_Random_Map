#pragma once

// 설명 :

class Vector2
{
public:
	int x;
	int y;
};
class RectInt
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
	int x;
	int y;
	int height;
	int width;
};

class Node
{
public:
	Node* leftNode;
	Node* rightNode;
	Node* parNode;
	RectInt nodeRect; //분리된 공간의 rect정보
	Node(RectInt rect)
	{
		nodeRect = rect;
		leftNode = nullptr;
		rightNode = nullptr;
		parNode = nullptr;
	}
};
