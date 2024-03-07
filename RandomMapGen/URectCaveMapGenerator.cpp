#include "URectCaveMapGenerator.h"

URectCaveMapGenerator::URectCaveMapGenerator() 
{
}

URectCaveMapGenerator::~URectCaveMapGenerator() 
{
}

bool URectCaveMapGenerator::CreateMap()
{
    URectMapGenerator::CreateMap();

    //CpyMap();

    Setting();

    for (int i = 0; i < 5; ++i)
    {
        ApplyRules();
    }
    //for (int i = 0; i < lx; ++i) // 랜덤맵 생성을 성공한 경우 cpy
    //{
    //    for (int j = 0; j < ly; ++j)
    //    {
    //        base_map[i][j] = try_map_gen[i][j];
    //    }
    //}


    // 노드 release
    LeafNodeList.clear();
    ReleaseNode(RootNode);
    for (Node* _node : ShapeList)
    {
        ReleaseNode(_node);
    }
    RootNode = nullptr;

    return true;

}


bool URectCaveMapGenerator::CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape)
{
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;
    CurMapShape = _Shape;

    return URectCaveMapGenerator::CreateMap();
}

void URectCaveMapGenerator::ApplyRules()
{
    CpyMap();
    const int dx[8] = { 0,0,1,-1 , -1, -1, 1,1 };
    const int dy[8] = { 1,-1,0,0 , -1, 1, -1,1 };
    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            int wallcnt = 0;
            for (int k = 0; k < 8; ++k)
            {
                int nx = i + dx[k], ny = j + dy[k];
                
                if (true == InRange(nx, ny))
                {
                    if (EMapGeneratorData::Wall == base_map[nx][ny])
                    {
                        wallcnt++;
                    }
                    else if (EMapGeneratorData::VoidTile == base_map[nx][ny])
                    {
                        wallcnt = 10;
                        break;
                    }

                }
                else
                {
                    wallcnt = 10;
                    break;
                }
            }
            if (wallcnt < 4)
            {
                try_map_gen[i][j] = EMapGeneratorData::Ground;
            }
            else
            {
                try_map_gen[i][j] = EMapGeneratorData::Wall;
            }
        }
    }
    for (int i = 0; i < lx; ++i) 
    {
        for (int j = 0; j < ly; ++j)
        {
            base_map[i][j] = try_map_gen[i][j];
        }
    }
}



void URectCaveMapGenerator::Setting()
{
    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            if (EMapGeneratorData::Ground == base_map[i][j])
            {
                if (0 == GameEngineRandom::MainRandom.RandomInt(0, 2))
                {
					base_map[i][j] = EMapGeneratorData::Wall;
                }
            }
        }
    }
}
