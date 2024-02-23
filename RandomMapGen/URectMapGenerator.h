#pragma once
#include <vector>
#include "Node.h"
#include "GameEngineRandom.h"

// ���� :
class URectMapGenerator
{
public:
	// constrcuter destructer
	URectMapGenerator();

	URectMapGenerator(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize)
	{
		base_map = _map;

		lx = static_cast<int>(_map.size());
		ly = static_cast<int>(_map[0].size());

		room_cnt = _roomcnt;
		min_room_size = _min_room_size;
		door_size = _doorsize;

		spare = 0.8f;

		RootNode = nullptr;

	}

	virtual ~URectMapGenerator();

	// delete Function
	URectMapGenerator(const URectMapGenerator& _Other) = delete;
	URectMapGenerator(URectMapGenerator&& _Other) noexcept = delete;
	URectMapGenerator& operator=(const URectMapGenerator& _Other) = delete;
	URectMapGenerator& operator=(URectMapGenerator&& _Other) noexcept = delete;


	void Print();
	virtual bool CreateMap();
	virtual bool CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize);

protected:


	// ��� �۾� - ��� ����
//
// x,y�� ���� map size �ε��� ���� �ִ��� Ȯ���ϴ� �۾�
	bool In_range(int x, int y);

	// ���� Node�� n���� ������ʹٴ� �ǹ�
	virtual bool DivideNode(Node* tree, int n, int _size, float _rate) = 0;

	//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
	virtual void DrawLine(const RectInt& _cur, int splite, bool is_height, int n) = 0;

	// ���� Rect�� ������ Left�� Right����� ũ�⸦ ������ �� �ֵ��� ��
	void GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right);




	// �����۾�
	// 
	// ���� Map������� �����۾��� �ϱ����� initialize
	void Init();

	// ���� ���� �κ�(-1)�� �׵θ��� ���� ����� �Լ�
	void SetWallBFS(int x, int y);

	// �ش� �ε������� ��� ������ ���� dp�� ����
	void CalMapSizeIndex();

	// RectInt�� �ش��ϴ� �κ��� ����
	int GetRoomSize(const RectInt Rectinfo);



	//�����۾�
	//
	// �׽�Ʈ�� �ʿ� cpy�ϴ� ����
	void CpyMap();

	// generate�Ϸ� �� ��带 �����ϱ� ����
	void ReleaseNode(Node* _cNode);








	std::vector<std::vector<EMapGeneratorData>>	base_map;
	std::vector<std::vector<bool>>	is_visited;		// SetWallBFS()���� �ߺ�üũ�� ���ϱ� ����
	std::vector<std::vector<int>>	map_size_Index;		// RectInt�� �ش��ϴ� �κ��� ���̸� ������ ���ϱ� ����
	std::vector<std::vector<EMapGeneratorData>>	try_map_gen;			// ������ ���� ���ɿ��θ� �׽�Ʈ�ϱ� ����

	int lx;		// ���� ���α���
	int ly;		// ���� ���α���

	int room_cnt;
	int min_room_size;
	float m_rate = 0.2f;		// ��带 �ڸ��� ���������� ���� 1 - m_rate ~ 1 + m_rate
	int door_size;		// �� �ڽĳ�� ���̸� �����ϴ� ���� ����
	float spare;			// ��带 �ڸ� �� �ּ� ���� ���� ���� ����

	Node* RootNode;		// ��Ʈ ���
	std::vector<Node*> LeafNodeList;	// ���ܳ��(���� ������� �� �ִ� ���)


private:
	
};

