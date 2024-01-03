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

	static void CreateMap(std::vector<std::vector<int>>& _Map, int roomcnt, int _size);

	static void SetWallDFS(int x, int y, std::vector<std::vector<bool>>& _isvisited, std::vector<std::vector<int>>& Map);
	static bool In_range(int x, int y,int n,int m);

	static void Divide(std::vector<std::vector<int>>& _Map, Node* tree,int n, int _size);
	static void DrawLine(std::vector<std::vector<int>>& _Map, Vector2 from, Vector2 to,int n); //from->to로 이어지는 선을 그리게 될 것이다.
	static void Print(const std::vector<std::vector<int>>& _Map);

	static int GetRoomSize(RectInt Rectinfo, const std::vector<std::vector<int>>& Map);

protected:

private:
	// 맵 크기 최대 최소비율 조정가능
	// 문 사이즈 조정가능
	// 방의 개수 조정 가능

	static float min_rate;
	static float max_rate;
	//static std::vector<std::vector<int>> Map;
	static int door_size;
};

