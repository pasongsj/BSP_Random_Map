#pragma once
#include "URectMapGenerator.h"

// 설명 :
class URectRoomMapGenerator : public URectMapGenerator
{
public:
	// constrcuter destructer
	URectRoomMapGenerator();

	~URectRoomMapGenerator();

	// delete Function
	URectRoomMapGenerator(const URectRoomMapGenerator& _Other) = delete;
	URectRoomMapGenerator(URectRoomMapGenerator&& _Other) noexcept = delete;
	URectRoomMapGenerator& operator=(const URectRoomMapGenerator& _Other) = delete;
	URectRoomMapGenerator& operator=(URectRoomMapGenerator&& _Other) noexcept = delete;

	// _map : 입력 맵, room cnt: 방의 개수 ,_min_room_size :최소 방의 크기, _doorsize : 통로와 문의 크기, _Shape : 전체 맵 모양
	bool CreateMap(std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;



protected:
	bool CreateMap() override;

	// 현재 Node를 n개로 나누고싶다는 의미
	bool DivideNode(Node* tree, int n, int _size, float _rate, bool is_reverse);

	//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
	bool DrawLine(const RectInt& _cur, int splite, bool is_height, int n) override;



private:


	// 방 모양 생성
	void CreateRoom(Node* _leafNode);

	// 방 연결
	bool ConnectRoom(Node* main_node, Node* sub_node = nullptr);


};

