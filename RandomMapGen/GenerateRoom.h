#pragma once
#include <vector>
#include "Node.h"

enum MapType
{
	WallType,
	RoomType
};

// ���� :
class GenerateRoom
{
public:
	// constrcuter destructer
	GenerateRoom();
	~GenerateRoom();

	// delete Function
	GenerateRoom(const GenerateRoom& _Other) = delete;
	GenerateRoom(GenerateRoom&& _Other) noexcept = delete;
	GenerateRoom& operator=(const GenerateRoom& _Other) = delete;
	GenerateRoom& operator=(GenerateRoom&& _Other) noexcept = delete;


	static void SetRate(float _Rate)
	{
		m_rate = _Rate;
	}

	static bool CreateMap(std::vector<std::vector<int>>& _Map, int roomcnt, int _size, MapType _Type);

	static void Print(const std::vector<std::vector<int>>& _Map);


protected:

private:


	// ��� �۾� - ��� ����
	//
	// x,y�� ���� map size �ε��� ���� �ִ��� Ȯ���ϴ� �۾�
	static bool In_range(int x, int y);

	// ���� Node�� n���� ������ʹٴ� �ǹ�
	static bool DivideNode(std::shared_ptr<Node> tree, int n, int _size, float _rate, MapType _Type);

	//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
	static void DrawLine(const RectInt& _cur, int splite, bool is_height, int n, MapType _Type);

	// ���� Rect�� ������ Left�� Right����� ũ�⸦ ������ �� �ֵ��� ��
	static void GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right);

	// ����۾� - �� ����
	static void CreateRoom(std::shared_ptr<Node> curNode);



	// �����۾�
	// 
	// ���� Map������� �����۾��� �ϱ����� initialize
	static void Init();

	// ���� ���� �κ�(-1)�� �׵θ��� ���� ����� �Լ�
	static void SetWallBFS(int x, int y, std::vector<std::vector<int>>& Map);

	// �ش� �ε������� ��� ������ ���� dp�� ����
	static void CalMapSizeIndex(const std::vector<std::vector<int>>& Map, std::vector<std::vector<int>>& MapIndex);

	// RectInt�� �ش��ϴ� �κ��� ����
	static int GetRoomSize(const RectInt Rectinfo);



	//�����۾�
	//
	// �׽�Ʈ�� �ʿ� cpy�ϴ� ����
	static void CpyMap(const std::vector<std::vector<int>>& _Map);
	
	// generate�Ϸ� �� ��带 �����ϱ� ����
	static void ReleaseNode(std::shared_ptr<Node> _cNode);


	// ����

	static std::vector<std::vector<bool>> isvisited;		// SetWallBFS()���� �ߺ�üũ�� ���ϱ� ����
	static std::vector<std::vector<int>>  MapSizeIndex;		// RectInt�� �ش��ϴ� �κ��� ���̸� ������ ���ϱ� ����
	static std::vector<std::vector<int>>  TryMap;			// ������ ���� ���ɿ��θ� �׽�Ʈ�ϱ� ����

	static int lx;		// ���� ���α���
	static int ly;		// ���� ���α���

	static float m_rate;		// ��带 �ڸ��� ���������� ���� 1 - m_rate ~ 1 + m_rate
	static int door_size;		// �� �ڽĳ�� ���̸� �����ϴ� ���� ����
	static float spare;			// ��带 �ڸ� �� �ּ� ���� ���� ���� ����

	static std::shared_ptr<Node> RootNode;		// ��Ʈ ���
	static std::vector<std::shared_ptr<Node>> LeafNodeList;	// ���ܳ��(���� ������� �� �ִ� ���)

};

