#include <vector>
#include <crtdbg.h>

#include "URectWallMapGenerator.h"
#include "URectRoomMapGenerator.h"
#include "URectCaveMapGenerator.h"


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::vector<std::vector<EMapGeneratorData>> Map(50, std::vector< EMapGeneratorData>(50, EMapGeneratorData::Ground));

	//int* a = new int();
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
	//	for (int j = 40; j < 50; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//	}
	//}
	//
	//for (int i = 40; i < 50; ++i)
	//{
	//	for (int j = 0; j < 10; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//	}
	//}
	//
	//for (int i = 40; i < 50; ++i)
	//{
	//	for (int j = 30; j < 40; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//	}
	//}


	URectMapGenerator* NewMap = new URectWallMapGenerator();
	if (true == NewMap->CreateMap(Map,30, 3, 2, MapShape::mieum))
	{
		NewMap->Print();
	}
	delete NewMap;


	return 0;
}