#pragma once
#include "URectMapGenerator.h"

// ���� : ref https://www.linkedin.com/pulse/cave-like-level-generation-using-cellular-automata-martin-celusniak/
class URectCaveMapGenerator : public URectMapGenerator
{
public:
	// constrcuter destructer
	URectCaveMapGenerator();
	~URectCaveMapGenerator();

	// delete Function
	URectCaveMapGenerator(const URectCaveMapGenerator& _Other) = delete;
	URectCaveMapGenerator(URectCaveMapGenerator&& _Other) noexcept = delete;
	URectCaveMapGenerator& operator=(const URectCaveMapGenerator& _Other) = delete;
	URectCaveMapGenerator& operator=(URectCaveMapGenerator&& _Other) noexcept = delete;


	// _map : �Է� ��, room cnt: ������� ,_min_room_size :(10~90%��) ground���� Ȯ��,_doorsize : �������, _Shape : ��ü �� ���
	bool CreateMap(std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;


protected:

	bool CreateMap() override;


private:
	// cell ��Ѹ���
	void Setting();
	// cell Ȯ�� �� ����
	void ApplyRules();
	// ������ ������ ������ Ȯ��
	bool ValidCheck();
	// ���� ������ ���
	int GetCaveSize(int x, int y, bool _remove = false);

	std::vector<std::vector<EMapGeneratorData>>	iter_map;			// ������ ���� ���ɿ��θ� �׽�Ʈ�ϱ� ����


};

