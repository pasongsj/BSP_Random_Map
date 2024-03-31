#include <vector>
#include <crtdbg.h>
#include <chrono>
#include <conio.h>

#include "URectWallMapGenerator.h"
#include "URectRoomMapGenerator.h"
#include "URectCaveMapGenerator.h"


#include <map>
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::vector<std::vector<EMapGeneratorData>> Map(40, std::vector< EMapGeneratorData>(40, EMapGeneratorData::Ground));

	//int* a = new int();
	// 
	// 
	int rate = 10;
	for (int i = 0; i < rate; ++i)
	{
		for (int j = 0; j < rate; ++j)
		{
			Map[i][j] = EMapGeneratorData::VoidTile;
			Map[i][Map[0].size()-j-1] = EMapGeneratorData::VoidTile;
			Map[Map.size()-1-i][j] = EMapGeneratorData::VoidTile;
			Map[Map.size()-1-i][Map[0].size() - j - 1] = EMapGeneratorData::VoidTile;
		}
	}


	URectMapGenerator* NewMap = nullptr;

	EMapType EType = EMapType::Cave;
	int Select = static_cast<int>(EType);


	while (true)
	{
		system("cls");

		std::cout << "�� Ÿ���� �������ּ��� : " << std::endl;
		std::cout << "1 ��" << std::endl;
		std::cout << "2 ��" << std::endl;
		std::cout << "3 ����" << std::endl;
		std::cin >> Select;
		EType = static_cast<EMapType>(Select - 1);

		switch (EType)
		{
		case EMapType::Wall:
		case EMapType::Room:
		case EMapType::Cave:
			break;
		default:
			return 0;
			break;
		}

		std::cout << "��ȸ �ݺ� �Ͻðڽ��ϱ�? : " << std::endl;
		std::cin >> Select;


		while (Select--)
		{
			GameEngineRandom::MainRandom.SetSeed(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

			switch (EType)
			{
			case EMapType::Wall:
			{
				NewMap = new URectWallMapGenerator();
				break;
			}
			case EMapType::Room:
			{
				NewMap = new URectRoomMapGenerator();
				break;
			}
			case EMapType::Cave:
			{
				NewMap = new URectCaveMapGenerator();
				break;
			}
			default:
				break;
			}


			switch (EType)
			{
			case EMapType::Wall:
			case EMapType::Room:
			{
				if (true == NewMap->CreateMap(Map, 30, 5, 1, MapShape::none))
				{
					NewMap->Print();
					std::cout << std::endl;
				}
				else {
					printf_s("Err");
				}
				break;
			}
			case EMapType::Cave:
			{
				if (true == NewMap->CreateCaveMap(Map,70, MapShape::none))
				{
					NewMap->Print();
					std::cout << std::endl;
				}
				else {
					printf_s("Err");
				}
				break;
			}
			default:
				break;
			}
			delete NewMap;
			NewMap = nullptr;
			
			
		}



		_getch();
	}


	return 0;
}