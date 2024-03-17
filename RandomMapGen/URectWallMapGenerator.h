#pragma once
#include "URectMapGenerator.h"

// ���� : ref https://www.roguebasin.com/index.php?title=Basic_BSP_Dungeon_generation
class URectWallMapGenerator : public URectMapGenerator
{
public:
	// constrcuter destructer
	URectWallMapGenerator();

	~URectWallMapGenerator();

	// delete Function
	URectWallMapGenerator(const URectWallMapGenerator& _Other) = delete;
	URectWallMapGenerator(URectWallMapGenerator&& _Other) noexcept = delete;
	URectWallMapGenerator& operator=(const URectWallMapGenerator& _Other) = delete;
	URectWallMapGenerator& operator=(URectWallMapGenerator&& _Other) noexcept = delete;

	// _map : �Է� ��, room cnt: ���� ���� ,_min_room_size :�ּ� ���� ũ��, _doorsize : ���� ũ��, _Shape : ��ü �� ���
	bool CreateMap(const std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;

protected:

	bool CreateMap() override;
	// ���� Node�� n���� ������ʹٴ� �ǹ�
	bool DivideNode(Node* tree, int n, int _size, float _rate);

	//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
	bool DrawLine(const RectInt& _cur, int splite, bool is_height, int n) override;


private:
	// ������ �ΰ��� node ��������
	bool MakeDoor(Node* _fnode, Node* _snode);
	// �� ���� �õ�
	bool TryDiv();

};

