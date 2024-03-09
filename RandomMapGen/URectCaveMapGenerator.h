#pragma once
#include "URectMapGenerator.h"

// 설명 : ref https://www.linkedin.com/pulse/cave-like-level-generation-using-cellular-automata-martin-celusniak/
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


	// _map : 입력 맵, room cnt: 상관없음 ,_min_room_size :(10~90%로) ground분포 확률,_doorsize : 상관없음, _Shape : 전체 맵 모양
	bool CreateMap(std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape = MapShape::none) override;


protected:

	bool CreateMap() override;


private:
	// cell 흩뿌리기
	void Setting();
	// cell 확장 룰 적용
	void ApplyRules();
	// 생성이 가능한 맵인지 확인
	bool ValidCheck();
	// 동굴 사이즈 얻기
	int GetCaveSize(int x, int y, bool _remove = false);

	std::vector<std::vector<EMapGeneratorData>>	iter_map;			// 랜덤맵 생성 가능여부를 테스트하기 위함


};

