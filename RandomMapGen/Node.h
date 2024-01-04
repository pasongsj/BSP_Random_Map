#pragma once

// ���� :


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
};
