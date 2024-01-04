#pragma once

// 설명 :


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

class Node
{
public:
	Node* parNode;			// 부모노드
	Node* leftNode;			// 왼쪽 자식 노드
	Node* rightNode;		// 오른쪽 자식 노드
	RectInt nodeRect;		// 공간 정보

	//생성자
	Node(RectInt rect)
	{
		nodeRect = rect;
		parNode = nullptr;
		leftNode = nullptr;
		rightNode = nullptr;
	}
};
