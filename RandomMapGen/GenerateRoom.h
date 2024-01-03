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

	static void Print(const std::vector<std::vector<int>>& _Map);


protected:

private:
	static void SetWallBFS(int x, int y, std::vector<std::vector<int>>& Map);
	static bool In_range(int x, int y);

	static void Divide(std::vector<std::vector<int>>& _Map, Node* tree,int n, int _size, float _rate);
	static void DrawLine(std::vector<std::vector<int>>& _Map, Vector2 from, Vector2 to,int n); //from->to로 이어지는 선을 그리게 될 것이다.
	static int GetRoomSize(const RectInt Rectinfo);
	static void CalMapSizeIndex(const std::vector<std::vector<int>>& Map, std::vector<std::vector<int>>& MapIndex);

	static float clamp(float _num)
	{
		if (_num < 0)
		{
			return 0;
		}
		else if (_num > 1)
		{
			return 1;
		}
		return _num;
	}

	static void GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right);


	static void Init();


	static std::vector<std::vector<bool>> isvisited;
	static std::vector<std::vector<int>>  MapSizeIndex;


	static int lx;
	static int ly;
	// 맵 크기 최대 최소비율 조정가능
	// 문 사이즈 조정가능
	// 방의 개수 조정 가능

	static float min_rate;
	static float max_rate;
	//static std::vector<std::vector<int>> MapSizeIndex;
	static int door_size;
};

