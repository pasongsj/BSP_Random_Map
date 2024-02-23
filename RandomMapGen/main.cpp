#include <RandomMapGen/GenerateRoom.h>
#include <vector>
#include <crtdbg.h>

#include "URectWallMapGenerator.h"
#include "URectRoomMapGenerator.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//int* a = new int();

	std::vector<std::vector<int>> Map;
	Map.resize(40);
	for (int i = 0; i < 40; ++i)
	{
		Map[i].resize(40);
	}
	
	//for (int i = 0; i < 15; ++i)
	//{
	//	for (int j = 0; j < 15; ++j)
	//	{
	//		Map[i][j] = -1;
	//	}
	//}
	//
	//for (int i = 0; i < 15; ++i)
	//{
	//	for (int j = 25; j < 40; ++j)
	//	{
	//		Map[i][j] = -1;
	//	}
	//}
	//
	//for (int i = 25; i < 40; ++i)
	//{
	//	for (int j = 0; j < 15; ++j)
	//	{
	//		Map[i][j] = -1;
	//	}
	//}
	//
	//for (int i = 25; i < 40; ++i)
	//{
	//	for (int j = 25; j < 40; ++j)
	//	{
	//		Map[i][j] = -1;
	//	}
	//}

	//URectMapGenerator(std::vector<std::vector<int>> _map, int _roomcnt, int _min_room_size, int _doorsize)

	URectMapGenerator* NewMap = new URectWallMapGenerator(Map, 20, 5, 1);
	if (true == NewMap->CreateMap())
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