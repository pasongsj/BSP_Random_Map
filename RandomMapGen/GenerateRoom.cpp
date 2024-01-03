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
    // bfs�� �̿��Ͽ� ���� ���� �κ� �ֺ��� ������ ���´�.
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

    // ���� ���� �κ�(-1)Ȯ��,
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
    if (n == 1) return; // ���̻� ���� ���� �ʿ䰡 ���� �� return;
    //�� ���� ��쿡��

    int maxLength = tree->nodeRect.width > tree->nodeRect.height ? tree->nodeRect.width : tree->nodeRect.height;;
    int split = maxLength/2;
    RectInt curRect = tree->nodeRect;
    int leftnodecnt = n / 2;
    int rightnodecnt = n - leftnodecnt;
    int max_room_size = static_cast<int>((_size + 1) * 2);



    int leftSize, rightSize;

    if (curRect.width >= curRect.height) //���ΰ� �� ����� ���
    {
        maxLength = tree->nodeRect.width;
        leftSize = GetRoomSize({ curRect.x + split + 1, curRect.y, curRect.width - split - 1, curRect.height });
        rightSize = GetRoomSize({ curRect.x, curRect.y, split, curRect.height });
        if (leftSize == 0)
        {
            tree->rightNode = new Node({ curRect.x, curRect.y, split, curRect.height });
            tree->rightNode->parNode = tree;
            Divide(_Map, tree->rightNode, n, _size);//����, ������ �ڽ� ���鵵 �����ش�.
            return;
        }
        else if (rightSize == 0)
        {
            tree->leftNode = new Node({ curRect.x + split + 1, curRect.y, curRect.width - split - 1, curRect.height });
            tree->leftNode->parNode = tree;
            Divide(_Map, tree->leftNode, n, _size);//����, ������ �ڽ� ���鵵 �����ش�.
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
            Divide(_Map, tree->rightNode, n, _size);//����, ������ �ڽ� ���鵵 �����ش�.
            return;
        }
        else if (rightSize == 0)
        {
            tree->leftNode = new Node({ curRect.x, curRect.y + split + 1, curRect.width, curRect.height - split - 1 });
            tree->leftNode->parNode = tree;
            Divide(_Map, tree->leftNode, n, _size);//����, ������ �ڽ� ���鵵 �����ش�.
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

    //���� �� �ִ� �ִ� ���̿� �ּ� �����߿��� �������� �� ���� ����

    

    if (curRect.width >= curRect.height) //���ΰ� �� ����� ��쿡�� �� ��� ������ �� ���̸�, �� ��쿡�� ���� ���̴� ������ �ʴ´�.
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

        // ���� �ּ� ũ�⸦ ���߱� �����̴�.

        tree->leftNode = new Node({ curRect.x, curRect.y, split, curRect.height});
        //���� ��忡 ���� ������ 
        //��ġ�� ���� �ϴ� �����̹Ƿ� ������ ������, ���� ���̴� ������ ���� �������� �־��ش�.

        tree->rightNode = new Node({ curRect.x + split+1, curRect.y, curRect.width - split-1, curRect.height });
        //���� ��忡 ���� ������ 
        //��ġ�� ���� �ϴܿ��� ���������� ���� ���̸�ŭ �̵��� ��ġ�̸�, ���� ���̴� ���� ���α��̿��� ���� ���� ���ΰ��� �� ������ �κ��� �ȴ�.

        DrawLine(_Map, { curRect.x + split, curRect.y }, { curRect.x + split, curRect.y + curRect.height },n);
        //�� �� �� �ΰ��� ��带 ������ ���� �׸��� �Լ��̴�.        
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
        // ���� �ּ� ũ�⸦ ���߱� �����̴�.

        tree->leftNode = new Node({ curRect.x, curRect.y, curRect.width, split });
        tree->rightNode = new Node({ curRect.x, curRect.y + split+1, curRect.width, curRect.height - split-1 });
		DrawLine(_Map, { curRect.x, curRect.y + split }, { curRect.x + curRect.width, curRect.y + split }, n);
        //���ΰ� �� ����� ����̴�. �ڼ��� ������ ���ο� ����.
    }
    tree->leftNode->parNode = tree; //�ڽĳ����� �θ��带 �������� ���� ����
    tree->rightNode->parNode = tree;
	Divide(_Map, tree->leftNode, leftnodecnt,_size); //����, ������ �ڽ� ���鵵 �����ش�.
	Divide(_Map, tree->rightNode, rightnodecnt, _size);//����, ������ �ڽ� ���鵵 �����ش�.
}


// �� : 1, �� : 0, �� : 2, ���� ���� : -1 �� ǥ��, (���3??�����)
void GenerateRoom::DrawLine(std::vector<std::vector<int>>& _Map, Vector2 from, Vector2 to,int n) //from->to�� �̾����� ���� �׸��� �� ���̴�.
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
