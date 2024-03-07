#pragma once
#include "URectMapGenerator.h"

// 설명 : ref https://www.roguebasin.com/index.php?title=Basic_BSP_Dungeon_generation
class URectWallMapGenerator : public URectMapGenerator
{
public:
	// constrcuter destructer
	URectWallMapGenerator();

	URectWallMapGenerator(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize)
	{
		base_map = _map;

		lx = static_cast<int>(_map.size());
		ly = static_cast<int>(_map[0].size());

		room_cnt = _roomcnt;
		min_room_size = _min_room_size;
		door_size = _doorsize;

		spare = 0.8f;

		RootNode = nullptr;
		CurMapShape = MapShape::none;

	}

	URectWallMapGenerator(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _shape)
	{
		base_map = _map;

		lx = static_cast<int>(_map.size());
		ly = static_cast<int>(_map[0].size());

		room_cnt = _roomcnt;
		min_room_size = _min_room_size;
		door_size = _doorsize;

		spare = 0.8f;

		RootNode = nullptr;
		CurMapShape = _shape;

	}

	~URectWallMapGenerator();

	// delete Function
	URectWallMapGenerator(const URectWallMapGenerator& _Other) = delete;
	URectWallMapGenerator(URectWallMapGenerator&& _Other) noexcept = delete;
	URectWallMapGenerator& operator=(const URectWallMapGenerator& _Other) = delete;
	URectWallMapGenerator& operator=(URectWallMapGenerator&& _Other) noexcept = delete;

	bool CreateMap() override;

	bool CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;

protected:

	// 현재 Node를 n개로 나누고싶다는 의미
	bool DivideNode(Node* tree, int n, int _size, float _rate) override;

	//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
	void DrawLine(const RectInt& _cur, int splite, bool is_height, int n) override;

	bool MakeDoor(Node* _fnode, Node* _snode);

private:

};

