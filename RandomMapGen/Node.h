#pragma once

#include <memory>
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
};