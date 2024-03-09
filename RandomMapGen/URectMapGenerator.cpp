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
            if (true == InRange(nx, ny))
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


bool URectMapGenerator::InRange(int x, int y)
{
    return 0 < x && x < lx  && 0 < y && y < ly;
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

void URectMapGenerator::RemoveRect(RectInt CurRect)
{
    for (int i = CurRect.x; i <= CurRect.x + CurRect.height; ++i)
    {
        for (int j = CurRect.y; j <= CurRect.y + CurRect.width; ++j)
        {
            base_map[i][j] = EMapGeneratorData::VoidTile;
        }
    }
}

void URectMapGenerator::FillTryMapRect(RectInt CurRect, EMapGeneratorData _data)
{
    for (int i = CurRect.x; i <= CurRect.x + CurRect.height; ++i)
    {
        for (int j = CurRect.y; j <= CurRect.y + CurRect.width; ++j)
        {
            try_map_gen[i][j] = _data;
        }
    }
}


void URectMapGenerator::DrawRect(RectInt& _CurRect)
{
    for (int i = _CurRect.x; i <= _CurRect.x + _CurRect.height; ++i)
    {
        base_map[i][_CurRect.y] = EMapGeneratorData::Wall;
        base_map[i][_CurRect.y+_CurRect.width] = EMapGeneratorData::Wall;
    }
    for (int j = _CurRect.y; j <= _CurRect.y + _CurRect.width; ++j)
    {
        base_map[_CurRect.x][j] = EMapGeneratorData::Wall;
        base_map[_CurRect.x+ _CurRect.height][j] = EMapGeneratorData::Wall;
    }
    _CurRect.x++;
    _CurRect.y++;
    _CurRect.height--;
    _CurRect.width--;
}


void URectMapGenerator::SettingMapShap()
{
    switch (CurMapShape)
    {
    case MapShape::none:
    {
        for (int i = 0; i < lx; ++i)
        {
            for (int j = 0; j < ly; ++j)
            {
                if (false == is_visited[i][j] && EMapGeneratorData::VoidTile == base_map[i][j])
                {
                    SetWallBFS(i, j);
                }
                else if (EMapGeneratorData::Ground == base_map[i][j] && (i == 0 || i == lx - 1 || j == 0 || j == ly - 1))
                {
                    base_map[i][j] = EMapGeneratorData::Wall;
                }
            }
        }
        break;
    }
    case MapShape::giyeok:
    {
        int split_x[3] = { 0, 0,lx - 1 };
        int split_y[3] = { 0, 0,ly - 1 };
        split_x[1] = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (lx / 2));

        split_y[1] = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (ly / 2));

        std::vector<RectInt> allRect;
        allRect.push_back({ split_x[0],split_y[0],split_x[1] - split_x[0],split_y[1] - split_y[0] });

        allRect.push_back({ split_x[0],split_y[1],split_x[1] - split_x[0],split_y[2] - split_y[1] });

        allRect.push_back({ split_x[1],split_y[1],split_x[2] - split_x[1],split_y[2] - split_y[1] });

        allRect.push_back({ split_x[1],split_y[0],split_x[2] - split_x[1],split_y[1] - split_y[0] });

        int RemovePos = GameEngineRandom::MainRandom.RandomInt(0, 3);

        RemoveRect(allRect[RemovePos]);

        for (int i = 1; i < 4; ++i)
        {
            Node* curNode = new Node();
            DrawRect(allRect[(RemovePos + i) % 4]);
            curNode->nodeRect = allRect[(RemovePos + i) % 4];
            ShapeList.push_back(curNode);
        }
        break;
    }
    case MapShape::digeut:
    case MapShape::mieum:
    case MapShape::cross:
    {
        int split_x[4] = { 0, 0,0,lx - 1 };
        int split_y[4] = { 0, 0,0,ly - 1 };
        split_x[1] = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (lx / 3));
        split_x[2] = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (lx * 2 / 3));

        split_y[1] = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (ly / 3));
        split_y[2] = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (ly * 2 / 3));

        std::vector<RectInt> allRect;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                allRect.push_back({ split_x[i],split_y[j],split_x[i + 1] - split_x[i],split_y[j + 1] - split_y[j] });
            }
        }

        int pos[8] = { 1,2,5,8,7,6,3,0 };
        switch (CurMapShape)
        {
        case MapShape::digeut:
        {
            int RemovePos = GameEngineRandom::MainRandom.RandomInt(0, 3) * 2;

            RemoveRect(allRect[pos[RemovePos]]);
            RemoveRect(allRect[4]);
            for (int i = 1; i < 8; ++i)
            {
                Node* curNode = new Node();
                DrawRect(allRect[pos[(i + RemovePos) % 8]]);
                curNode->nodeRect = allRect[pos[(i + RemovePos) % 8]];
                ShapeList.push_back(curNode);
            }

            break;
        }
        case MapShape::mieum:
        {
            RemoveRect(allRect[4]);
            for (int i = 0; i < 8; ++i)
            {
                Node* curNode = new Node();
                DrawRect(allRect[pos[i]]);
                curNode->nodeRect = allRect[pos[i]];
                ShapeList.push_back(curNode);
            }
            break;
        }
        case MapShape::cross:
        {

            RemoveRect(allRect[0]);
            RemoveRect(allRect[2]);
            RemoveRect(allRect[6]);
            RemoveRect(allRect[8]);
            {
                Node* curNode = new Node();
                DrawRect(allRect[4]);
                curNode->nodeRect = allRect[4];
                ShapeList.push_back(curNode);
            }
            for (int i = 1; i < 8; i += 2)
            {
                Node* curNode = new Node();
                DrawRect(allRect[i]);
                curNode->nodeRect = allRect[i];
                ShapeList.push_back(curNode);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}


bool URectMapGenerator::CreateMap()
{
    Init();
    SettingMapShap();


    //// 맵 크기를 빠르게 계산하기 위함
    CalMapSizeIndex();

    //CpyMap();


    return true;
}

bool URectMapGenerator::CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _shape)
{
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;
    CurMapShape = _shape;

    return CreateMap();
}



#include <map>

void URectMapGenerator::Print()
{
    std::map< EMapGeneratorData, std::string> printmatch = { {EMapGeneratorData::Ground,"□ "},{EMapGeneratorData::Wall, "벽"},{EMapGeneratorData::Door,"문"},{EMapGeneratorData::VoidTile,"X "},{EMapGeneratorData::Passage, "통"}};
        for (int i = 0; i < base_map.size(); ++i)
        {
            for (int j = 0; j < base_map[0].size(); ++j)
            {
                std::cout << printmatch[base_map[i][j]] << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';

}
