#pragma once
#include <vector>
#include "Node.h"

// 설명 :
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

	static void CreateMap(std::vector<std::vector<int>>& _Map, int roomcnt, int _size);

	static void Print(const std::vector<std::vector<int>>& _Map);


protected:

private:
	// 땅이 없는 부분(-1)의 테두리에 벽을 세우는 함수
	static void SetWallBFS(int x, int y, std::vector<std::vector<int>>& Map);

	// x,y가 현재 map size 인덱스 내에 있는지 확인하는 작업
	static bool In_range(int x, int y);

	// 현재 Node를 n개로 나누고싶다는 의미
	static bool Divide(std::shared_ptr<Node> tree, int n, int _size, float _rate);

	//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
	static void DrawLine(const RectInt& _cur, int splite, bool is_height, int n);

	// 해당 인덱스까지 모든 넓이의 합을 dp로 구함
	static void CalMapSizeIndex(const std::vector<std::vector<int>>& Map, std::vector<std::vector<int>>& MapIndex);
	// RectInt에 해당하는 부분의 넓이
	static int GetRoomSize(const RectInt Rectinfo);

	// 현재 Rect를 나누어 Left와 Right노드의 크기를 가져올 수 있도록 함
	static void GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right);


	// 현재 Map사이즈로 사전작업을 하기위한 initialize
	static void Init();

	// 테스트용 맵에 cpy하는 과정
	static void CpyMap(const std::vector<std::vector<int>>& _Map);

	// generate완료 후 노드를 삭제하기 위함
	static void ReleaseNode(std::shared_ptr<Node> _cNode);



	static std::vector<std::vector<bool>> isvisited;		// SetWallBFS()에서 중복체크를 피하기 위함
	static std::vector<std::vector<int>>  MapSizeIndex;		// RectInt에 해당하는 부분의 넓이를 빠르게 구하기 위함
	static std::vector<std::vector<int>>  TryMap;			// 랜덤맵 생성 가능여부를 테스트하기 위함

	static int lx;		// 맵의 세로길이
	static int ly;		// 맵의 가로길이

	static float m_rate;		// 노드를 자르는 랜덤비율의 범위 1 - m_rate ~ 1 + m_rate
	static int door_size;		// 두 자식노드 사이를 연결하는 문의 길이
	static float spare;			// 노드를 자를 때 최소 여분 영역 제공 비율

	static std::shared_ptr<Node> RootNode;		// 루트 노드
};

