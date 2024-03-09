#pragma once
#include "URectMapGenerator.h"

// ���� :
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

	// _map : �Է� ��, room cnt: ���� ���� ,_min_room_size :�ּ� ���� ũ��, _doorsize : ��ο� ���� ũ��, _Shape : ��ü �� ���
	bool CreateMap(std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;



protected:
	bool CreateMap() override;

	// ���� Node�� n���� ������ʹٴ� �ǹ�
	bool DivideNode(Node* tree, int n, int _size, float _rate, bool is_reverse);

	//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
	bool DrawLine(const RectInt& _cur, int splite, bool is_height, int n) override;



private:


	// �� ��� ����
	void CreateRoom(Node* _leafNode);

	// �� ����
	bool ConnectRoom(Node* main_node, Node* sub_node = nullptr);


};

