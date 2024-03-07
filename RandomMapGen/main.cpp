#include <RandomMapGen/GenerateRoom.h>
#include <vector>
#include <crtdbg.h>

#include "URectWallMapGenerator.h"
#include "URectRoomMapGenerator.h"
#include "URectCaveMapGenerator.h"


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//int* a = new int();

	std::vector<std::vector<EMapGeneratorData>> Map(50,std::vector< EMapGeneratorData>(50, EMapGeneratorData::Ground));
	//Map.resize(40);
	//for (int i = 0; i < 40; ++i)
	//{
	//	Map[i].resize(40);
	//}
	
	//for (int i = 0; i < 10; ++i)
	//{
	//	for (int j = 0; j < 10; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//	}
	//}
	//
	//for (int i = 0; i < 10; ++i)
	//{
	//	for (int j = 30; j < 40; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//	}
	//}
	//
	//for (int i = 30; i < 40; ++i)
	//{
	//	for (int j = 0; j < 10; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//	}
	//}
	//
	//for (int i = 30; i < 40; ++i)
	//{
	//	for (int j = 30; j < 40; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//	}
	//}

	//URectMapGenerator(std::vector<std::vector<int>> _map, int _roomcnt, int _min_room_size, int _doorsize)

	URectMapGenerator* NewMap = new URectCaveMapGenerator();
	if (true == NewMap->CreateMap(Map,16, 3, 1, MapShape::none))
	{
		NewMap->Print();
	}
	delete NewMap;
	//GenerateRoom* newMap = new GenerateRoom();

	//if (true == newMap->CreateMap(Map, 20, 5, MapType::RoomType))
	//{
	//	newMap->Print(Map);
	//}

	//delete newMap;

	return 0;
}