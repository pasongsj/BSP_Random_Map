#pragma once
#include "URectMapGenerator.h"

// Ό³Έν :
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

	bool CreateMap() override;
	bool CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize) override;

protected:
	//bool DivideNode(Node* tree, int n, int _size, float _rate) override;
	//bool DivideNode(Node* tree, int n, int _size, float _rate, bool is_reverse);


private:

};

