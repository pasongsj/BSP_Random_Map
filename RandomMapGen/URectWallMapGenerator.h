#pragma once
#include "URectMapGenerator.h"

// 설명 : ref https://www.roguebasin.com/index.php?title=Basic_BSP_Dungeon_generation
class URectWallMapGenerator : public URectMapGenerator
{
public:
	// constrcuter destructer
	URectWallMapGenerator();

	~URectWallMapGenerator();

	// delete Function
	URectWallMapGenerator(const URectWallMapGenerator& _Other) = delete;
	URectWallMapGenerator(URectWallMapGenerator&& _Other) noexcept = delete;
	URectWallMapGenerator& operator=(const URectWallMapGenerator& _Other) = delete;
	URectWallMapGenerator& operator=(URectWallMapGenerator&& _Other) noexcept = delete;

	// _map : 입력 맵, room cnt: 방의 개수 ,_min_room_size :최소 방의 크기, _doorsize : 문의 크기, _Shape : 전체 맵 모양
	bool CreateMap(const std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;

protected:

	bool CreateMap() override;
	// 현재 Node를 n개로 나누고싶다는 의미
	bool DivideNode(Node* tree, int n, int _size, float _rate);

	//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
	bool DrawLine(const RectInt& _cur, int splite, bool is_height, int n) override;


private:
	// 인접한 두개의 node 공간연결
	bool MakeDoor(Node* _fnode, Node* _snode);
	// 맵 생성 시도
	bool TryDiv();

};

