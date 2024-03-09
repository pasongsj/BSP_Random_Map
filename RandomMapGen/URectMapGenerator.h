#pragma once
#include <vector>
#include <set>
#include "Node.h"
#include "GameEngineRandom.h"


// ���� :
class URectMapGenerator
{
public:
	// constrcuter destructer
	URectMapGenerator();

	virtual ~URectMapGenerator();

	// delete Function
	URectMapGenerator(const URectMapGenerator& _Other) = delete;
	URectMapGenerator(URectMapGenerator&& _Other) noexcept = delete;
	URectMapGenerator& operator=(const URectMapGenerator& _Other) = delete;
	URectMapGenerator& operator=(URectMapGenerator&& _Other) noexcept = delete;


	void Print();

	virtual bool CreateMap(std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _shape = MapShape::none) = 0;

	void SettingMapShap();

	// room type ignore 
	enum class RoomType
	{
		None,
		Rect,
		Triangle,
		Rhombus,
		Circle,
	};
	void SetIgnoreRoomType(RoomType _Type)
	{
		IgnoreRoomType.insert(_Type);
	}

	void SetIgnoreRoomType(const std::vector<RoomType>& _TypeVec)
	{
		for (RoomType _Type : _TypeVec)
		{
			IgnoreRoomType.insert(_Type);
		}
	}

protected:

	virtual bool CreateMap();

	// ��� �۾� - ��� ����
	//
	// x,y�� ���� map size �ε��� ���� �ִ��� Ȯ���ϴ� �۾�
	bool InRange(int x, int y);

	// ���� Node�� n���� ������ʹٴ� �ǹ�
	//virtual bool DivideNode(Node* tree, int n, int _size, float _rate) { return false; };

	//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
	virtual bool DrawLine(const RectInt& _cur, int splite, bool is_height, int n) { return false; };

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
	std::vector<Node*> ShapeList;

	// RectInt�� �ش��ϴ� �κ��� void tile ó��
	void RemoveRect(RectInt CurRect);
	// RectInt�� �ش��ϴ� �׵θ��� Wall ó��
	void DrawRect(RectInt& CurRect);
	// RectInt�� �ش��ϴ� �κ��� voidtile�� �����ϰ� data�� ����
	void FillTryMapRect(RectInt CurRect,EMapGeneratorData _data);

	MapShape CurMapShape;



	//room type ingore
		// �������� ���� ���� ��� ����Ʈ
	std::vector<RoomType> RoomTypeList;
	std::set<RoomType>	IgnoreRoomType;

private:
	
};

