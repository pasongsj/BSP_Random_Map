#include "GenerateRoom.h"
#include "GameEngineRandom.h"

#include <iostream>
#include <queue>
#include <tuple>


GenerateRoom* GenerateRoom::Room = nullptr;

float GenerateRoom::min_rate = 0.8f;
float GenerateRoom::max_rate = 1.2f;
int GenerateRoom::door_size = 1;


std::vector<std::vector<bool>> GenerateRoom::isvisited;
std::vector<std::vector<int>>  GenerateRoom::MapSizeIndex;

int GenerateRoom::lx = 0;
int GenerateRoom::ly = 0;

//std::vector<std::vector<int>> GenerateRoom::MapSizeIndex;


GenerateRoom::GenerateRoom() 
{
    Room = this;
}

GenerateRoom::~GenerateRoom() 
{
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
            isvisited[i][j] = 0;
        }
    }
}


bool GenerateRoom::In_range(int x, int y)
{
    return 0 < x && x < lx-1 && 0 < y && y < ly-1;
}
int GenerateRoom::GetRoomSize(const RectInt Rectinfo)
{
    return MapSizeIndex[Rectinfo.x + Rectinfo.width - 1][Rectinfo.y + Rectinfo.height - 1] - MapSizeIndex[Rectinfo.x - 1][Rectinfo.y + Rectinfo.height - 1]
        - MapSizeIndex[Rectinfo.x + Rectinfo.width - 1][Rectinfo.y - 1] + MapSizeIndex[Rectinfo.x - 1][Rectinfo.y - 1];
}


void GenerateRoom::SetWallBFS(int x, int y, std::vector<std::vector<int>>& Map)
{
    // bfs를 이용하여 맵이 없는 부분 주변을 벽으로 막는다.
    std::queue<std::pair<int, int>> dfsq;
    dfsq.push(std::make_pair(x, y));
    isvisited[x][y] = true;
	const int dx[4] = { 0,0,1,-1 };
	const int dy[4] = { 1,-1,0,0 };

    while (!dfsq.empty())
    {
        int cx, cy;
        std::tie(cx, cy) = dfsq.front();
        dfsq.pop();

        for (int i = 0; i < 4; ++i)
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


void GenerateRoom::CreateMap(std::vector<std::vector<int>>& _Map, int roomcnt, int _size)
{
    //Map = _Map;
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
            if (_Map[i][j] == -1)
            {
                SetWallBFS(i, j, _Map);
            }
			else if (0 == _Map[i][j] && (i == 0 || i == lx - 1 || j == 0 || j == ly - 1))
            {
                _Map[i][j] = 1;
            }
            isvisited[i][j] = true;
        }
    }

    CalMapSizeIndex(_Map, MapSizeIndex);

    Node* RootNode = new Node({1, 1, lx -1, ly -1});
    Divide(_Map, RootNode, roomcnt, _size);
}


void GenerateRoom::Divide(std::vector<std::vector<int>>& _Map, Node* tree, int n, int _size)
{
    if (n == 1) return; // 더이상 방을 나눌 필요가 없을 때 return;
    //그 외의 경우에는

    int maxLength = tree->nodeRect.width > tree->nodeRect.height ? tree->nodeRect.width : tree->nodeRect.height;;
    int split = maxLength/2;
    RectInt curRect = tree->nodeRect;
    int leftnodecnt = n / 2;
    int rightnodecnt = n - leftnodecnt;
    int max_room_size = static_cast<int>((_size + 1) * 2);



    int leftSize, rightSize;

    if (curRect.width >= curRect.height) //가로가 더 길었던 경우
    {
        maxLength = tree->nodeRect.width;
        leftSize = GetRoomSize({ curRect.x + split + 1, curRect.y, curRect.width - split - 1, curRect.height });
        rightSize = GetRoomSize({ curRect.x, curRect.y, split, curRect.height });
        if (leftSize == 0)
        {
            tree->rightNode = new Node({ curRect.x, curRect.y, split, curRect.height });
            tree->rightNode->parNode = tree;
            Divide(_Map, tree->rightNode, n, _size);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
            return;
        }
        else if (rightSize == 0)
        {
            tree->leftNode = new Node({ curRect.x + split + 1, curRect.y, curRect.width - split - 1, curRect.height });
            tree->leftNode->parNode = tree;
            Divide(_Map, tree->leftNode, n, _size);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
            return;
        }
    }
    else
    {
        maxLength = tree->nodeRect.height;
        leftSize = GetRoomSize({ curRect.x, curRect.y + split + 1, curRect.width, curRect.height - split - 1 });
        rightSize = GetRoomSize({ curRect.x, curRect.y, curRect.width, split });
        if (leftSize == 0)
        {
            tree->rightNode = new Node({ curRect.x, curRect.y, curRect.width, split });
            tree->rightNode->parNode = tree;
            Divide(_Map, tree->rightNode, n, _size);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
            return;
        }
        else if (rightSize == 0)
        {
            tree->leftNode = new Node({ curRect.x, curRect.y + split + 1, curRect.width, curRect.height - split - 1 });
            tree->leftNode->parNode = tree;
            Divide(_Map, tree->leftNode, n, _size);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
            return;
        }
    }
    
    float room_size_rate = static_cast<float>(leftSize ) /(leftSize + rightSize);

    if (n % 2 == 1)
    {
        split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(min_rate, max_rate) * maxLength * 0.67f * room_size_rate);
    }
    else
    {
        split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(min_rate, max_rate) * maxLength * room_size_rate);
    }

    //나올 수 있는 최대 길이와 최소 길이중에서 랜덤으로 한 값을 선택

    

    if (curRect.width >= curRect.height) //가로가 더 길었던 경우에는 좌 우로 나누게 될 것이며, 이 경우에는 세로 길이는 변하지 않는다.
    {
		if (n > 3)
        {
			int trycnt = 0;
            while (true)
            {
                trycnt++;
                RectInt tmpleft = { curRect.x, curRect.y, split, curRect.height };
                RectInt tmpright = { curRect.x + split + 1, curRect.y, curRect.width - split - 1, curRect.height };
                if (max_room_size * leftnodecnt > GetRoomSize(tmpleft) && max_room_size * rightnodecnt > GetRoomSize(tmpright))
                {
                    printf("ERRRRRRRR");
                    return;
                }
                if (trycnt > static_cast<int>(maxLength * max_rate))
                {
                    break;
                }
                else if (max_room_size * leftnodecnt > GetRoomSize(tmpleft))
                {
                    split++;
                }
                else if (max_room_size * rightnodecnt > GetRoomSize(tmpright))
                {
                    split--;
                }
                else
                {
                    break;
                }
            }
        }

        // 방의 최소 크기를 맞추기 위함이다.

        tree->leftNode = new Node({ curRect.x, curRect.y, split, curRect.height});
        //왼쪽 노드에 대한 정보다 
        //위치는 좌측 하단 기준이므로 변하지 않으며, 가로 길이는 위에서 구한 랜덤값을 넣어준다.

        tree->rightNode = new Node({ curRect.x + split+1, curRect.y, curRect.width - split-1, curRect.height });
        //우측 노드에 대한 정보다 
        //위치는 좌측 하단에서 오른쪽으로 가로 길이만큼 이동한 위치이며, 가로 길이는 기존 가로길이에서 새로 구한 가로값을 뺀 나머지 부분이 된다.

        DrawLine(_Map, { curRect.x + split, curRect.y }, { curRect.x + split, curRect.y + curRect.height },n);
        //그 후 위 두개의 노드를 나눠준 선을 그리는 함수이다.        
    }
    else
    {
        if (n > 3)
		{
			int trycnt = 0;
			while (true)
			{
				trycnt++;
				RectInt tmpleft = { curRect.x, curRect.y, curRect.width, split };
				RectInt tmpright = { curRect.x, curRect.y + split + 1, curRect.width, curRect.height - split - 1 };
				if (max_room_size * leftnodecnt > GetRoomSize(tmpleft) && max_room_size * rightnodecnt > GetRoomSize(tmpright))
				{
					printf("ERRRRRRRR");
                    return;
				}
				if (trycnt > static_cast<int>(maxLength * max_rate))
				{
					break;
				}
				else if (max_room_size * leftnodecnt > GetRoomSize(tmpleft))
				{
					split++;
				}
				else if (max_room_size * rightnodecnt > GetRoomSize(tmpright))
				{
					split--;
				}
				else
				{
					break;
				}
			}
		}
        // 방의 최소 크기를 맞추기 위함이다.

        tree->leftNode = new Node({ curRect.x, curRect.y, curRect.width, split });
        tree->rightNode = new Node({ curRect.x, curRect.y + split+1, curRect.width, curRect.height - split-1 });
		DrawLine(_Map, { curRect.x, curRect.y + split }, { curRect.x + curRect.width, curRect.y + split }, n);
        //세로가 더 길었던 경우이다. 자세한 사항은 가로와 같다.
    }
    tree->leftNode->parNode = tree; //자식노드들의 부모노드를 나누기전 노드로 설정
    tree->rightNode->parNode = tree;
	Divide(_Map, tree->leftNode, leftnodecnt,_size); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
	Divide(_Map, tree->rightNode, rightnodecnt, _size);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
}


// 벽 : 1, 땅 : 0, 문 : 2, 없는 지역 : -1 로 표기, (통로3??고민중)
void GenerateRoom::DrawLine(std::vector<std::vector<int>>& _Map, Vector2 from, Vector2 to,int n) //from->to로 이어지는 선을 그리게 될 것이다.
{
    
    if (from.x == to.x)
    {
        int starty = from.y > to.y ? to.y : from.y;
        int endy = from.y < to.y ? to.y : from.y;
        int diff = static_cast<int>((endy - starty) * 0.7f)/2/n;
        int mid = (starty + endy) / 2;
		int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - diff, mid + diff);
        for (; starty < endy; ++starty)
        {
            if (0 == _Map[from.x][starty])
            {
                _Map[from.x][starty] = 1;
            }
        }
        //Map[from.x][doorPos] = 2;

        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
        {
            if (_Map[from.x][i] != -1)
            {
                _Map[from.x][i] = 2;
            }
        }
    }
    else if (from.y == to.y)
    {
        int startx = from.x > to.x ? to.x : from.x;
        int endx = from.x < to.x ? to.x : from.x;
        int diff = static_cast<int>((endx - startx) * 0.7f)/2/n;
        int mid = (startx + endx) / 2;
        int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - diff, mid + diff);
        for (; startx < endx; ++startx)
        {
            if (0 == _Map[startx][from.y])
            {
                _Map[startx][from.y] = 1;
            }
        }

        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size&1); ++i)
        {
            if (_Map[i][from.y] != -1)
            {
                _Map[i][from.y] = 2;
            }
        }
    }


}

void GenerateRoom::Print(const std::vector<std::vector<int>>& _Map)
{
    for (int i = 0; i < 60; ++i)
    {
        for (int j = 0; j < 50; ++j)
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
