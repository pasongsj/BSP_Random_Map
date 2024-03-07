#pragma once
#include "URectMapGenerator.h"

// ���� : ref https://www.roguebasin.com/index.php?title=Basic_BSP_Dungeon_generation
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

	// ���� Node�� n���� ������ʹٴ� �ǹ�
	bool DivideNode(Node* tree, int n, int _size, float _rate) override;

	//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
	void DrawLine(const RectInt& _cur, int splite, bool is_height, int n) override;

	bool MakeDoor(Node* _fnode, Node* _snode);

private:

};

