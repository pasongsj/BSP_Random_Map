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
	// 
	// 
	//int rate = 15;
	//for (int i = 0; i < rate; ++i)
	//{
	//	for (int j = 0; j < rate; ++j)
	//	{
	//		Map[i][j] = EMapGeneratorData::VoidTile;
	//		Map[i][Map[0].size()-j-1] = EMapGeneratorData::VoidTile;
	//		Map[Map.size()-1-i][j] = EMapGeneratorData::VoidTile;
	//		Map[Map.size()-1-i][Map[0].size() - j - 1] = EMapGeneratorData::VoidTile;
	//	}
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


	URectMapGenerator* NewMap = new URectCaveMapGenerator();
	if (true == NewMap->CreateMap(Map,30, 70, 1, MapShape::mieum))
	{
		NewMap->Print();
	}
	delete NewMap;


	return 0;
}