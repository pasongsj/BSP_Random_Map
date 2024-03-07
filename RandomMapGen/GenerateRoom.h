#pragma once
#include <vector>
#include "Node.h"

enum class MapType
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


	void SetRate(float _Rate)
	{
		m_rate = _Rate;
	}

	bool CreateMap(std::vector<std::vector<int>>& _Map, int roomcnt, int _size, MapType _Type);

	void Print(const std::vector<std::vector<int>>& _Map);


protected:

private:


	// ��� �۾� - ��� ����
	//
	// x,y�� ���� map size �ε��� ���� �ִ��� Ȯ���ϴ� �۾�
	bool InRange(int x, int y);

	// ���� Node�� n���� ������ʹٴ� �ǹ�
	bool DivideNode(std::shared_ptr<Node> tree, int n, int _size, float _rate, MapType _Type);

	//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
	void DrawLine(const RectInt& _cur, int splite, bool is_height, int n, MapType _Type);

	// ���� Rect�� ������ Left�� Right����� ũ�⸦ ������ �� �ֵ��� ��
	void GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right);

	// ����۾� - �� ����
	void CreateRoom(std::shared_ptr<Node> curNode);



	// �����۾�
	// 
	// ���� Map������� �����۾��� �ϱ����� initialize
	void Init();

	// ���� ���� �κ�(-1)�� �׵θ��� ���� ����� �Լ�
	void SetWallBFS(int x, int y, std::vector<std::vector<int>>& Map);

	// �ش� �ε������� ��� ������ ���� dp�� ����
	void CalMapSizeIndex(const std::vector<std::vector<int>>& Map, std::vector<std::vector<int>>& MapIndex);

	// RectInt�� �ش��ϴ� �κ��� ����
	int GetRoomSize(const RectInt Rectinfo);



	//�����۾�
	//
	// �׽�Ʈ�� �ʿ� cpy�ϴ� ����
	void CpyMap(const std::vector<std::vector<int>>& _Map);
	
	// generate�Ϸ� �� ��带 �����ϱ� ����
	void ReleaseNode(std::shared_ptr<Node> _cNode);


	// ����

	std::vector<std::vector<bool>> isvisited;		// SetWallBFS()���� �ߺ�üũ�� ���ϱ� ����
	std::vector<std::vector<int>>  MapSizeIndex;		// RectInt�� �ش��ϴ� �κ��� ���̸� ������ ���ϱ� ����
	std::vector<std::vector<int>>  TryMap;			// ������ ���� ���ɿ��θ� �׽�Ʈ�ϱ� ����

	int lx;		// ���� ���α���
	int ly;		// ���� ���α���

	float m_rate = 0.2f;		// ��带 �ڸ��� ���������� ���� 1 - m_rate ~ 1 + m_rate
	int door_size = 1;		// �� �ڽĳ�� ���̸� �����ϴ� ���� ����
	float spare = 0.8f;			// ��带 �ڸ� �� �ּ� ���� ���� ���� ����

	std::shared_ptr<Node> RootNode;		// ��Ʈ ���
	std::vector<std::shared_ptr<Node>> LeafNodeList;	// ���ܳ��(���� ������� �� �ִ� ���)

};

