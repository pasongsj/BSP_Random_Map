

#include <RandomMapGen/GenerateRoom.h>
#include <iostream>
#include <vector>

int main()
{
	std::vector<std::vector<int>> Map;
	Map.resize(60);
	for (int i = 0; i < 60; ++i)
	{
		Map[i].resize(50);
	}
	GenerateRoom* now = new GenerateRoom();
	GenerateRoom::CreateMap(Map, 7);

	for (int i = 0; i < 60; ++i)
	{
		for (int j = 0; j < 50; ++j)
		{
			std::cout << Map[i][j] << ' ';

		}
		std::cout << '\n';
	}

	return 0;
}