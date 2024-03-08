#pragma once
#include "URectMapGenerator.h"

// 설명 :
class URectRoomMapGenerator : public URectMapGenerator
{
public:
	// constrcuter destructer
	URectRoomMapGenerator();
	URectRoomMapGenerator(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize)
	{
		base_map = _map;

		lx = static_cast<int>(_map.size());
		ly = static_cast<int>(_map[0].size());

		room_cnt = _roomcnt;
		min_room_size = _min_room_size;
		door_size = _doorsize;

		spare = 0.8f;

		RootNode = nullptr;
		CurMapShape = MapShape::none;
	}
	URectRoomMapGenerator(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _shape)
	{
		base_map = _map;

		lx = static_cast<int>(_map.size());
		ly = static_cast<int>(_map[0].size());

		room_cnt = _roomcnt;
		min_room_size = _min_room_size;
		door_size = _doorsize;

		spare = 0.8f;

		RootNode = nullptr;
		CurMapShape = _shape;

	}
	~URectRoomMapGenerator();

	// delete Function
	URectRoomMapGenerator(const URectRoomMapGenerator& _Other) = delete;
	URectRoomMapGenerator(URectRoomMapGenerator&& _Other) noexcept = delete;
	URectRoomMapGenerator& operator=(const URectRoomMapGenerator& _Other) = delete;
	URectRoomMapGenerator& operator=(URectRoomMapGenerator&& _Other) noexcept = delete;

	bool CreateMap() override;
	//bool CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize) override;
	bool CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;


protected:

	// 현재 Node를 n개로 나누고싶다는 의미
	bool DivideNode(Node* tree, int n, int _size, float _rate) override;
	bool DivideNode(Node* tree, int n, int _size, float _rate, bool is_reverse);

	//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
	bool DrawLine(const RectInt& _cur, int splite, bool is_height, int n) override;

	//bool MakeRoad(const RectInt f_rect,const RectInt s_rect);


private:

	void CreateRoom(Node* _leafNode);
	bool ConnectRoom(Node* main_node, Node* sub_node = nullptr);

};

