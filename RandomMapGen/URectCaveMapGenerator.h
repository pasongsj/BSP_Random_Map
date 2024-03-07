#pragma once
#include "URectMapGenerator.h"

// Ό³Έν : ref https://www.linkedin.com/pulse/cave-like-level-generation-using-cellular-automata-martin-celusniak/
class URectCaveMapGenerator : public URectMapGenerator
{
public:
	// constrcuter destructer
	URectCaveMapGenerator();
	URectCaveMapGenerator(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize)
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
	URectCaveMapGenerator(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _shape)
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
	~URectCaveMapGenerator();

	// delete Function
	URectCaveMapGenerator(const URectCaveMapGenerator& _Other) = delete;
	URectCaveMapGenerator(URectCaveMapGenerator&& _Other) noexcept = delete;
	URectCaveMapGenerator& operator=(const URectCaveMapGenerator& _Other) = delete;
	URectCaveMapGenerator& operator=(URectCaveMapGenerator&& _Other) noexcept = delete;

	bool CreateMap() override;

	bool CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;

protected:



private:

	void Setting();
	void ApplyRules();

};

