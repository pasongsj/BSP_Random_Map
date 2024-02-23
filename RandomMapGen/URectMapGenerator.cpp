#include "URectMapGenerator.h"
#include <queue>
#include <tuple>

URectMapGenerator::URectMapGenerator() 
{
}

URectMapGenerator::~URectMapGenerator() 
{
}

void URectMapGenerator::Init()
{
    is_visited.clear();
    is_visited = std::vector<std::vector<bool>>(lx, std::vector<bool>(ly, false));
    map_size_Index.clear();
    map_size_Index = std::vector<std::vector<int>>(lx, std::vector<int>(ly, 0));
}


void URectMapGenerator::SetWallBFS(int x, int y)
{
    // bfs를 이용하여 맵이 없는 부분 주변을 벽으로 막는다.
    std::queue<std::pair<int, int>> dfsq;
    dfsq.push(std::make_pair(x, y));
    is_visited[x][y] = true;
    const int dx[8] = { 0,0,1,-1 , -1, -1, 1,1 };
    const int dy[8] = { 1,-1,0,0 , -1, 1, -1,1 };

    while (!dfsq.empty())
    {
        int cx, cy;
        std::tie(cx, cy) = dfsq.front();
        dfsq.pop();

        for (int i = 0; i < 8; ++i)
        {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (true == In_range(nx, ny))
            {
                if (EMapGeneratorData::Ground == base_map[nx][ny])
                {
                    base_map[nx][ny] = EMapGeneratorData::Wall;
                }
                else if (EMapGeneratorData ::VoidTile == base_map[nx][ny] && false == is_visited[nx][ny])
                {
                    dfsq.push(std::make_pair(nx, ny));
                    is_visited[nx][ny] = true;
                }
            }
        }

    }
}


void URectMapGenerator::CalMapSizeIndex()
{
    for (int i = 1; i < lx; ++i)
    {
        for (int j = 1; j < ly; ++j)
        {
            map_size_Index[i][j] = map_size_Index[i - 1][j] + map_size_Index[i][j - 1] - map_size_Index[i - 1][j - 1];
            if (EMapGeneratorData::Ground == base_map[i][j])
            {
                map_size_Index[i][j]++;
            }
        }
    }
}

int URectMapGenerator::GetRoomSize(const RectInt Rectinfo)
{
    return map_size_Index[Rectinfo.x + Rectinfo.height - 1][Rectinfo.y + Rectinfo.width - 1] - map_size_Index[Rectinfo.x - 1][Rectinfo.y + Rectinfo.width - 1]
        - map_size_Index[Rectinfo.x + Rectinfo.height - 1][Rectinfo.y - 1] + map_size_Index[Rectinfo.x - 1][Rectinfo.y - 1];
}

void URectMapGenerator::CpyMap()
{
    try_map_gen.resize(lx);
    for (int i = 0; i < lx; ++i)
    {
        try_map_gen[i].resize(ly);
        for (int j = 0; j < ly; ++j)
        {
            try_map_gen[i][j] = base_map[i][j];
        }
    }
}



void URectMapGenerator::ReleaseNode(Node* _cNode)
{
    if (nullptr == _cNode)
    {
        return;
    }
    if (_cNode->leftNode != nullptr)
    {
        ReleaseNode(_cNode->leftNode);
        _cNode->leftNode = nullptr;
    }
    if (_cNode->rightNode != nullptr)
    {
        ReleaseNode(_cNode->rightNode);
        _cNode->rightNode = nullptr;
    }
    delete _cNode;
}


bool URectMapGenerator::In_range(int x, int y)
{
    return 0 < x && x < lx - 1 && 0 < y && y < ly - 1;
}

void URectMapGenerator::GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right)
{
    if (true == is_height)
    {
        Left = { _cur.x, _cur.y, _split, _cur.width };
        Right = { _cur.x + _split + 1, _cur.y, _cur.height - _split - 1, _cur.width };
    }
    else
    {
        Left = { _cur.x, _cur.y, _cur.height, _split };
        Right = { _cur.x, _cur.y + _split + 1, _cur.height, _cur.width - _split - 1 };
    }
}



bool URectMapGenerator::CreateMap()
{
    Init();
    // 맵이 없는 부분(-1)확인,
    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            if (true == is_visited[i][j])
            {
                continue;
            }

            if (EMapGeneratorData::VoidTile == base_map[i][j])// -1인 부분이 있다면 BFS를 통해 주변에 벽을 세움
            {
                SetWallBFS(i, j);
            }
            else if (EMapGeneratorData::Ground == base_map[i][j] && (i == 0 || i == lx - 1 || j == 0 || j == ly - 1)) // 테두리부분에도 벽을 세움
            {
                base_map[i][j] = EMapGeneratorData::Wall;
            }
            is_visited[i][j] = true;
        }
    }

    // 맵 크기를 빠르게 계산하기 위함
    CalMapSizeIndex();

    return true;
}

bool URectMapGenerator::CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize)
{
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;


    return CreateMap();
}


#include <map>

void URectMapGenerator::Print()
{
    std::map< EMapGeneratorData, std::string> printmatch = { {EMapGeneratorData::Ground,"땅"},{EMapGeneratorData::Wall, "벽"},{EMapGeneratorData::Door,"문"},{EMapGeneratorData::VoidTile,"X"},{EMapGeneratorData::Passage, "통"}};
        for (int i = 0; i < base_map.size(); ++i)
        {
            for (int j = 0; j < base_map[0].size(); ++j)
            {
                std::cout << printmatch[base_map[i][j]] << ' ';
            }
            std::cout << '\n';
        }
}
