#pragma once
#include <vector>
#include "Node.h"

// 설명 :
class GenerateRoom
{
	static GenerateRoom* Room;
public:
	// constrcuter destructer
	GenerateRoom();
	~GenerateRoom();

	// delete Function
	GenerateRoom(const GenerateRoom& _Other) = delete;
	GenerateRoom(GenerateRoom&& _Other) noexcept = delete;
	GenerateRoom& operator=(const GenerateRoom& _Other) = delete;
	GenerateRoom& operator=(GenerateRoom&& _Other) noexcept = delete;


	static void SetMinRate(float _Min)
	{
		min_rate = _Min;
	}
	
	static void SetMaxRate(float _Max)
	{
		max_rate = _Max;
	}

	static void CreateMap(std::vector<std::vector<int>> _Map, int _maxdepth);

	static void SetWallDFS(int x, int y, std::vector<std::vector<bool>>& _isvisited);
	static bool In_range(int x, int y,int n,int m);

	static void Divide(Node* tree,int n);
	static void DrawLine(Vector2 from, Vector2 to,int n); //from->to로 이어지는 선을 그리게 될 것이다.
	static void Print();



protected:

private:
	static float min_rate;
	static float max_rate;
	static int maxdepth;
	static std::vector<std::vector<int>> Map;
};

