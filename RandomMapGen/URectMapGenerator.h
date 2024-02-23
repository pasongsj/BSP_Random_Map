#pragma once
#include <vector>
#include "Node.h"
#include "GameEngineRandom.h"

// 설명 :
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


	// 노드 작업 - 노드 제작
//
// x,y가 현재 map size 인덱스 내에 있는지 확인하는 작업
	bool In_range(int x, int y);

	// 현재 Node를 n개로 나누고싶다는 의미
	virtual bool DivideNode(Node* tree, int n, int _size, float _rate) = 0;

	//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
	virtual void DrawLine(const RectInt& _cur, int splite, bool is_height, int n) = 0;

	// 현재 Rect를 나누어 Left와 Right노드의 크기를 가져올 수 있도록 함
	void GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right);




	// 사전작업
	// 
	// 현재 Map사이즈로 사전작업을 하기위한 initialize
	void Init();

	// 땅이 없는 부분(-1)의 테두리에 벽을 세우는 함수
	void SetWallBFS(int x, int y);

	// 해당 인덱스까지 모든 넓이의 합을 dp로 구함
	void CalMapSizeIndex();

	// RectInt에 해당하는 부분의 넓이
	int GetRoomSize(const RectInt Rectinfo);



	//정리작업
	//
	// 테스트용 맵에 cpy하는 과정
	void CpyMap();

	// generate완료 후 노드를 삭제하기 위함
	void ReleaseNode(Node* _cNode);








	std::vector<std::vector<EMapGeneratorData>>	base_map;
	std::vector<std::vector<bool>>	is_visited;		// SetWallBFS()에서 중복체크를 피하기 위함
	std::vector<std::vector<int>>	map_size_Index;		// RectInt에 해당하는 부분의 넓이를 빠르게 구하기 위함
	std::vector<std::vector<EMapGeneratorData>>	try_map_gen;			// 랜덤맵 생성 가능여부를 테스트하기 위함

	int lx;		// 맵의 세로길이
	int ly;		// 맵의 가로길이

	int room_cnt;
	int min_room_size;
	float m_rate = 0.2f;		// 노드를 자르는 랜덤비율의 범위 1 - m_rate ~ 1 + m_rate
	int door_size;		// 두 자식노드 사이를 연결하는 문의 길이
	float spare;			// 노드를 자를 때 최소 여분 영역 제공 비율

	Node* RootNode;		// 루트 노드
	std::vector<Node*> LeafNodeList;	// 말단노드(방이 만들어질 수 있는 노드)


private:
	
};

