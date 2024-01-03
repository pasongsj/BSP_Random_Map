#pragma once
#include <vector>
#include "Node.h"

// ���� :
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
	static void DrawLine(std::vector<std::vector<int>>& _Map, Vector2 from, Vector2 to,int n); //from->to�� �̾����� ���� �׸��� �� ���̴�.
	static void Print(const std::vector<std::vector<int>>& _Map);

	static int GetRoomSize(RectInt Rectinfo, const std::vector<std::vector<int>>& Map);

protected:

private:
	// �� ũ�� �ִ� �ּҺ��� ��������
	// �� ������ ��������
	// ���� ���� ���� ����

	static float min_rate;
	static float max_rate;
	//static std::vector<std::vector<int>> Map;
	static int door_size;
};

