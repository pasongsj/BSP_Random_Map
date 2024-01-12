#include "GenerateRoom.h"
#include "GameEngineRandom.h"

#include <iostream>
#include <queue>
#include <tuple>



float GenerateRoom::m_rate = 0.2f;
int GenerateRoom::door_size = 1;
float GenerateRoom::spare = 1.0f;
std::shared_ptr<class Node> GenerateRoom::RootNode = nullptr;


std::vector<std::vector<bool>> GenerateRoom::isvisited;
std::vector<std::vector<int>>  GenerateRoom::MapSizeIndex;
std::vector<std::vector<int>>  GenerateRoom::TryMap;
std::vector<std::shared_ptr<Node>> GenerateRoom::LeafNodeList;

int GenerateRoom::lx = 0;
int GenerateRoom::ly = 0;



GenerateRoom::GenerateRoom() 
{
    //Room = this;
}

GenerateRoom::~GenerateRoom() 
{
}

void GenerateRoom::ReleaseNode(std::shared_ptr<Node> _cNode)
{
    LeafNodeList.clear();
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
    if (_cNode->parNode != nullptr)
    {
        _cNode->parNode = nullptr;
    }
    //delete _cNode;
}


void GenerateRoom::Init()
{
    isvisited.resize(lx);
    MapSizeIndex.resize(lx);
    for (int i = 0; i < lx; ++i)
    {
        isvisited[i].resize(ly);
        MapSizeIndex[i].resize(ly);
        for (int j = 0; j < ly; ++j)
        {
            isvisited[i][j] = false;
        }
    }
}

void GenerateRoom::CpyMap(const std::vector<std::vector<int>>& _Map)
{
	TryMap.resize(lx);
	for (int i = 0; i < lx; ++i)
	{
		TryMap[i].resize(ly);
		for (int j = 0; j < ly; ++j)
		{
			TryMap[i][j] = _Map[i][j];
		}
	}
}


bool GenerateRoom::In_range(int x, int y)
{
    return 0 < x && x < lx-1 && 0 < y && y < ly-1;
}
int GenerateRoom::GetRoomSize(const RectInt Rectinfo)
{
    return MapSizeIndex[Rectinfo.x + Rectinfo.height - 1][Rectinfo.y + Rectinfo.width - 1] - MapSizeIndex[Rectinfo.x - 1][Rectinfo.y + Rectinfo.width - 1]
        - MapSizeIndex[Rectinfo.x + Rectinfo.height - 1][Rectinfo.y - 1] + MapSizeIndex[Rectinfo.x - 1][Rectinfo.y - 1];
}

void GenerateRoom::SetWallBFS(int x, int y, std::vector<std::vector<int>>& Map)
{
    // bfs를 이용하여 맵이 없는 부분 주변을 벽으로 막는다.
    std::queue<std::pair<int, int>> dfsq;
    dfsq.push(std::make_pair(x, y));
    isvisited[x][y] = true;
	const int dx[8] = { 0,0,1,-1 , -1, -1, 1,1};
	const int dy[8] = { 1,-1,0,0 , -1, 1, -1,1};

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
                if (0 == Map[nx][ny])
                {
                    Map[nx][ny] = 1;
                }
                else if (-1 == Map[nx][ny] && false == isvisited[nx][ny])
                {
                    dfsq.push(std::make_pair(nx, ny));
                    isvisited[nx][ny] = true;
                }
            }
        }

    }
}

void GenerateRoom::CalMapSizeIndex(const std::vector<std::vector<int>>& Map, std::vector<std::vector<int>>& MapIndex)
{
    for (int i = 1; i < lx; ++i)
    {
        for (int j = 1; j < ly; ++j)
        {
            MapIndex[i][j] = MapIndex[i - 1][j] + MapIndex[i][j - 1] - MapIndex[i - 1][j - 1];
            if (Map[i][j] == 0)
            {
                MapIndex[i][j]++;
            }
        }
    }
}

bool GenerateRoom::CreateMap(std::vector<std::vector<int>>& _Map, int roomcnt, int _size, MapType _Type)
{
    ///Map = _Map;
    lx = static_cast<int>(_Map.size());
    ly = static_cast<int>(_Map[0].size());

    Init();

    // 맵이 없는 부분(-1)확인,
    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            if (true == isvisited[i][j])
            {
                continue;
            }

            if (_Map[i][j] == -1)// -1인 부분이 있다면 BFS를 통해 주변에 벽을 세움
            {
                SetWallBFS(i, j, _Map);
            }
            else if (0 == _Map[i][j] && (i == 0 || i == lx - 1 || j == 0 || j == ly - 1)) // 테두리부분에도 벽을 세움
            {
                _Map[i][j] = 1;
            }
            isvisited[i][j] = true;
        }
    }

    // 맵 크기를 빠르게 계산하기 위함
    CalMapSizeIndex(_Map, MapSizeIndex);

    // 전체 넓이 / 최대 방크기+테두리 비율
    spare = 0.8f;
    float ratio = static_cast<float>(GetRoomSize({ 1, 1, lx - 1, ly - 1 })) / ((_size + 1) * 2 * roomcnt);


    // spare을 늘려가며 가능한 랜덤맵이 있는지 확인한다.
    while (true)
    {
        // 테스트할 맵을 복사한다
        CpyMap(_Map);

        // 방 크기의 여유 비율
        float _rate = ratio * spare + 0.1f < 1.0f ? 1.0f : ratio * spare + 0.1f;

        // 맵 생성
        if (nullptr == RootNode)
        {
            RootNode = std::make_shared<Node>();
            RootNode->nodeRect = { 1, 1, lx - 1, ly - 1 }; // 맵의 테두리를 제외한 크기의 루트노드
        }
        bool can_gen = DivideNode(RootNode, roomcnt, _size, _rate,_Type);

        if (true == can_gen) // 랜덤맵 생성 성공
        {
            break;
        }
        else
        {
            // 노드 release
            ReleaseNode(RootNode);
            RootNode = nullptr;
            spare += 0.01f;
            if (spare >= 1.0f) // 랜덤맵 생성 실패
            {
                printf("err");
                return false;
            }
        }
    }

    switch (_Type)
    {
    case WallType:
        break;
    case RoomType:
        // CreateRoom
        // ConnectRoom
        break;
    default:
        break;
    }

    for (int i = 0; i < lx; ++i) // 랜덤맵 생성을 성공한 경우 cpy
    {
        for (int j = 0; j < ly; ++j)
        {
            _Map[i][j] = TryMap[i][j];
        }
    }

    // -- test --
    // 
    std::shared_ptr<Node> curNode = RootNode->MaxNode();
    int cnt = 1;
    //while (nullptr != curNode)
    //{
    //    std::cout << cnt++<<' ';
    //    curNode->PrintNode();
    //    curNode = curNode->GetBeforeNode();
    //}
    //std::cout << '\n';

    for (const std::shared_ptr<Node>& CurNode : LeafNodeList)
    {
        std::cout << cnt++ << ' ';
        CurNode->PrintNode();
    }
    std::cout << '\n';


    // -- test --


    // 노드 release
    ReleaseNode(RootNode);
    RootNode = nullptr;

    return true;
}


void GenerateRoom::CreateRoom(std::shared_ptr<Node> curNode)
{
    int Size = GetRoomSize(curNode->nodeRect);
}




void GenerateRoom::GetChildRect(const RectInt& _cur, int _split, bool is_height, RectInt& Left, RectInt& Right)
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


bool GenerateRoom::DivideNode(std::shared_ptr<Node> tree, int n, int _size, float _rate, MapType _Type)
{
    if (n == 1) // 더이상 방을 나눌 필요가 없을 때
    {
        if (_size > GetRoomSize(tree->nodeRect)) // 최소 방 크기를 만족하지 않는다면
        {
            return false;
        }
        LeafNodeList.push_back(tree);
        return true;
    }


    // 방을 더 나눠야 할 때

    // 현재 노드의 방 정보
    const RectInt curRect = tree->nodeRect;

    // 가로로 나눌지 세로로 나눌지
    int maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;;
    bool is_height = curRect.height > curRect.width ? true : false;

    // 5:5 비율로 나눈다고 가정
    int split = maxLength / 2;

    // 왼쪽노드의 방 개수
    int leftnodecnt = n / 2;
    // 오른쪽 노드의 방 개수
    int rightnodecnt = n - leftnodecnt;

    // 최대 방 크기 * 여분 비율
    int max_room_size = static_cast<int>((_size + 1) * 2 * _rate);


    // 5:5로 나누었을 때 Left와 Right임시값
    RectInt LeftRect, RightRect;
    GetChildRect(curRect, split, is_height, LeftRect, RightRect);

    int leftSize = GetRoomSize(LeftRect);
    int rightSize = GetRoomSize(RightRect);

    // 한쪽이 모두 -1인경우 방을 만들 수 없기 때문에 반대쪽 노드에 n개만큼 생성한다.
    if (leftSize == 0)
    {
        tree->rightNode = std::make_shared<Node>();
        tree->rightNode->nodeRect = RightRect;
        tree->rightNode->parNode = tree;
        return DivideNode(tree->rightNode, n, _size, _rate, _Type);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
    else if (rightSize == 0)
    {
        tree->leftNode = std::make_shared<Node>();
        tree->leftNode->nodeRect = LeftRect;
        tree->leftNode->parNode = tree;
        return DivideNode(tree->leftNode, n, _size, _rate,_Type);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }


    // 홀수개의 방을 만들어야하는 경우 1 : 2 크기 비율로 split를 계산하기 위함
    if (n % 2 == 1)
    {
        int tmp = (maxLength * leftSize) / (leftSize + 2 * rightSize);
        split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (tmp));
    }
    else
    {
        int tmp = (maxLength * leftSize) / (leftSize + rightSize);
        split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (tmp));
    }

    // split 위치가 사각형을 벗어난 경우
    if (split < 1 || split > maxLength - 1)
    {
        return false;
    }

    // 나올 수 있는 최대 길이와 최소 길이중에서 랜덤으로 한 값을 선택


    {
        // 방의 여분 비율을 맞추기 위해 split을 조정하는 과정
        int trycnt = 0;
        while (true)
        {
            trycnt++;
            if (trycnt > static_cast<int>(maxLength * (1.0f + m_rate)))
            {
                break;
            }
            GetChildRect(curRect, split, is_height, LeftRect, RightRect);
            leftSize = GetRoomSize(LeftRect);
            rightSize = GetRoomSize(RightRect);
            if (max_room_size * leftnodecnt > leftSize && max_room_size * rightnodecnt > rightSize)
            {
                return false;
            }
            else if (max_room_size * leftnodecnt > leftSize)
            {
                split++;
            }
            else if (max_room_size * rightnodecnt > rightSize)
            {
                split--;
            }
            else
            {
                break;
            }
        }
        // 방의 최소 크기를 맞추기 위함이다.

        //왼쪽 노드에 대한 정보다 
        tree->leftNode = std::make_shared<Node>();
        tree->leftNode->nodeRect = LeftRect;

        //우측 노드에 대한 정보다 
        tree->rightNode = std::make_shared<Node>();
        tree->rightNode->nodeRect = RightRect;

        //그 후 위 두개의 노드를 나눠준 선을 그리는 함수이다.        
        DrawLine(curRect, split, is_height, n, _Type);
    }

    tree->leftNode->parNode = tree; //자식노드들의 부모노드를 나누기전 노드로 설정
    tree->rightNode->parNode = tree;

    // 여분비율을 낮춘다
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;

    return DivideNode(tree->leftNode, leftnodecnt, _size, nrate,_Type) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate, _Type); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
}


void GenerateRoom::DrawLine(const RectInt& _cur, int splite, bool is_height, int n, MapType _Type)
{
	if (is_height)
	{
        int mid = _cur.y + _cur.width / 2;
        int diff = static_cast<int>(_cur.width * 0.35f / n);
		int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - diff, mid + diff);
		for (int i = _cur.y; i < _cur.y + _cur.width; ++i)
		{
			if (0 == TryMap[_cur.x + splite][i])
			{
                TryMap[_cur.x + splite][i] = 1;
			}
		}

        if(MapType::WallType == _Type)
        {
            for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
            {
                if (TryMap[_cur.x + splite][i] != -1)
                {
                    TryMap[_cur.x + splite][i] = 2;
                }
            }
        }

	}
	else
	{
        int mid = _cur.x + _cur.height / 2;
        int diff = static_cast<int>(_cur.height * 0.35f / n);
        int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - diff, mid + diff);
		for (int i = _cur.x; i < _cur.x + _cur.height; ++i)
		{
			if (0 == TryMap[i][_cur.y + splite])
			{
                TryMap[i][_cur.y + splite] = 1;
			}
		}
        if (MapType::WallType == _Type)
        {
            for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
            {
                if (TryMap[i][_cur.y + splite] != -1)
                {
                    TryMap[i][_cur.y + splite] = 2;
                }
            }
        }
	}
}



// 벽 : 1, 땅 : 0, 문 : 2, 없는 지역 : -1 로 표기
void GenerateRoom::Print(const std::vector<std::vector<int>>& _Map)
{
    for (int i = 0; i < _Map.size(); ++i)
    {
        for (int j = 0; j < _Map[0].size(); ++j)
        {
            if (_Map[i][j] >= 0)
            {
                std::cout << ' ';
            }
            std::cout << _Map[i][j] << ' ';
        }
        std::cout << '\n';
    }
}
