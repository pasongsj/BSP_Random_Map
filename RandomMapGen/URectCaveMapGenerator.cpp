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

    CpyMap();

    Setting();
    for (int i = 0; i < 4; ++i)
    {
        ApplyRules();
    }


    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            base_map[i][j] = try_map_gen[i][j];
        }
    }

    // ³ëµå release
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
    iter_map.resize(lx);
    for (int i = 0; i < lx; ++i)
    {
        iter_map[i].resize(ly);
        for (int j = 0; j < ly; ++j)
        {
            iter_map[i][j] = try_map_gen[i][j];
        }
    }

    const int dx[8] = { 0,0,1,-1 , -1, -1, 1,1 };
    const int dy[8] = { 1,-1,0,0 , -1, 1, -1,1 };
    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            if (EMapGeneratorData::VoidTile == try_map_gen[i][j])
            {
                continue;   
            }
            int wallcnt = 0;
            for (int k = 0; k < 8; ++k)
            {
                int nx = i + dx[k], ny = j + dy[k];

                if (true == InRange(nx, ny))
                {
                    if (EMapGeneratorData::Wall == try_map_gen[nx][ny])
                    {
                        wallcnt++;
                    }
                    else if (EMapGeneratorData::VoidTile == try_map_gen[nx][ny])
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
                iter_map[i][j] = EMapGeneratorData::Ground;
            }
            else
            {
                iter_map[i][j] = EMapGeneratorData::Wall;
            }
        }
    }
    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            try_map_gen[i][j] = iter_map[i][j];
        }
    }
}



void URectCaveMapGenerator::Setting()
{
    const int dx[8] = { 0,  0,  1, -1 , -1, -1,  1,  1 };
    const int dy[8] = { 1, -1,  0,  0 , -1,  1, -1,  1 };

    for (int i = 1; i < lx-1; ++i)
    {
        for (int j = 1; j < ly-1; ++j)
        {
            if (EMapGeneratorData::VoidTile == try_map_gen[i][j])
            {
                continue;
            }

            bool is_def = false;
            for (int k = 0; k < 8; ++k)
            {
                int nx = i + dx[k], ny = j + dy[k];

				if (!(0 <= nx && nx < lx && 0 <= ny && ny < ly) || EMapGeneratorData::VoidTile == try_map_gen[nx][ny])
                {
                    is_def = true;
                    break;
                }
            }
            if (true == is_def || 0 == GameEngineRandom::MainRandom.RandomInt(0, 2))
            {
                try_map_gen[i][j] = EMapGeneratorData::Wall;
            }
            else
            {
                try_map_gen[i][j] = EMapGeneratorData::Ground;
            }

        }
    }
}
