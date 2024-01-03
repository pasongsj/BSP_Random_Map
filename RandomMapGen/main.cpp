#include <RandomMapGen/GenerateRoom.h>
#include <vector>

int main()
{
	std::vector<std::vector<int>> Map;
	Map.resize(60);
	for (int i = 0; i < 60; ++i)
	{
		Map[i].resize(50);
	}

	for (int i = 0; i < 30; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			Map[i][j] = -1;
			//Map[59 - i][49 - j] = -1;
		}
	}

	for (int i = 40; i < 60; ++i)
	{
		for (int j = 25; j < 50; ++j)
		{
			Map[i][j] = -1;
			//Map[59 - i][49 - j] = -1;
		}
	}
	//Map[3][5] = -1;
	//Map[3][4] = -1;
	//Map[3][3] = -1;
	//Map[34][23] = -1;
	//Map[33][22] = -1;
	//Map[56][2] = -1;
	//Map[8][2] = -1;
	GenerateRoom::CreateMap(Map, 50,5);
	GenerateRoom::Print(Map);

	

	return 0;
}