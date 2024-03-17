#include "URectCaveMapGenerator.h"
#include <queue>
#include <tuple>

URectCaveMapGenerator::URectCaveMapGenerator() 
{
}

URectCaveMapGenerator::~URectCaveMapGenerator() 
{
}

int URectCaveMapGenerator::GetCaveSize(int x, int y, bool _remove)
{
    int ans = 0;

    std::queue <std::pair<int, int>> q;

    if (false == _remove)
    {
        try_map_gen[x][y] = EMapGeneratorData::Wall;
    }
    else
    {
		is_visited[x][y] = true;
    }
    q.push(std::make_pair(x, y));
    while (!q.empty())
    {
        int cx, cy;
        std::tie(cx, cy) = q.front();
        q.pop();
        ans++;
        const int dx[4] = { 0,0,1,-1 };
        const int dy[4] = { 1,-1,0,0 };
        for (int k = 0; k < 4; ++k)
        {
            int nx = cx + dx[k];
            int ny = cy + dy[k];

            if(false == _remove)
            {
                if (true == InRange(nx, ny) && false == is_visited[nx][ny] && EMapGeneratorData::Ground == try_map_gen[nx][ny])
                {
                    is_visited[nx][ny] = true;
                    q.push(std::make_pair(nx, ny));
                }
            }
            else
            {
                if (true == InRange(nx, ny) && EMapGeneratorData::Ground == try_map_gen[nx][ny])
                {
                    try_map_gen[nx][ny] = EMapGeneratorData::Wall;
                    q.push(std::make_pair(nx, ny));
                }
            }
        }
    }
    return ans;
}

bool URectCaveMapGenerator::ValidCheck()
{
    is_visited.clear();
    is_visited = std::vector<std::vector<bool>>(lx, std::vector<bool>(ly, false));

    int cur_cave_size = 0;
    int last_x = -1, last_y = -1;
    for (int i = 1; i < lx - 1; ++i)
    {
        for (int j = 1; j < ly - 1; ++j)
        {
            if (EMapGeneratorData::Ground == try_map_gen[i][j] && false == is_visited[i][j])
            {
                int n_cave = GetCaveSize(i, j);
                if (0 == cur_cave_size)
                {
                    cur_cave_size = n_cave;
                    last_x = i;
                    last_y = j;
                }
                else if (cur_cave_size < n_cave)
                {
                    GetCaveSize(last_x, last_y, true);
                    last_x = i;
                    last_y = j;
                    cur_cave_size = n_cave;
                }
                else
                {
                    GetCaveSize(i, j,true);
                }
            }
        }
    }

    int MaxMapsize = GetRoomSize({ 1,1,lx - 1,ly - 1 });
    if (cur_cave_size * 100 / MaxMapsize < min_room_size)
    {
        return false;
    }
    return true;
}


bool URectCaveMapGenerator::CreateMap()
{
    URectMapGenerator::CreateMap();
    if (min_room_size < 10 || min_room_size > 90)
    {
        min_room_size = 50;
    }
    CpyMap();

    // 맵 생성 트라이

    int trycnt = 0;
    while (true)
    {
        trycnt++;
        Setting();
        for (int i = 0; i < 4; ++i)
        {
            ApplyRules();
        }

        if (true == ValidCheck())
        {
            break;
        }
        if (trycnt > min_room_size*3)
        {
            LeafNodeList.clear();
            ReleaseNode(RootNode);
            for (Node* _node : ShapeList)
            {
                ReleaseNode(_node);
            }
            RootNode = nullptr;
            return false;
        }

    }

    // 생성 된 맵 복사하기
    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            base_map[i][j] = try_map_gen[i][j];
        }
    }

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


bool URectCaveMapGenerator::CreateMap(const std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape)
{
    isDone = false;
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;
    CurMapShape = _Shape;


    if (true == URectCaveMapGenerator::CreateMap())
    {
        //for (int i = 0; i < lx; ++i)
        //{
        //    for (int j = 0; j < ly; ++j)
        //    {
        //        _map[i][j] = base_map[i][j];
        //    }
        //}
        //isDone = true;
        return isDone = true, true;
    }
    isDone = true;
    return false;
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
