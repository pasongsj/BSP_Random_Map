#include "GenerateRoom.h"
#include "GameEngineRandom.h"

#include <iostream>
#include <queue>
#include <tuple>


GenerateRoom* GenerateRoom::Room = nullptr;

float GenerateRoom::min_rate = 0.9f;
float GenerateRoom::max_rate = 1.1f;
int GenerateRoom::door_size = 1;
float GenerateRoom::Rate = 1.0f;


std::vector<std::vector<bool>> GenerateRoom::isvisited;
std::vector<std::vector<int>>  GenerateRoom::MapSizeIndex;
std::vector<std::vector<int>>  GenerateRoom::TryMap;

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

void GenerateRoom::Init(const std::vector<std::vector<int>>& _Map)
{
    isvisited.resize(lx);
    MapSizeIndex.resize(lx);
    TryMap.resize(lx);
    for (int i = 0; i < lx; ++i)
    {
        isvisited[i].resize(ly);
        MapSizeIndex[i].resize(ly);
        TryMap[i].resize(ly);
        for (int j = 0; j < ly; ++j)
        {
            isvisited[i][j] = false;
            isvisited[i][j] = 0;
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

    Init(_Map);

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

    Rate = 0.83f;
    CalMapSizeIndex(_Map, MapSizeIndex);
    Node* RootNode = new Node({1, 1, lx -1, ly -1});
    float _rate = static_cast<float>(GetRoomSize(RootNode->nodeRect)) / ((_size + 1) * 2 * roomcnt);
    _rate = _rate * Rate < 1.0f ? 1.0f : _rate * Rate;
	Divide(_Map, RootNode, roomcnt, _size, _rate);
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


void GenerateRoom::Divide(std::vector<std::vector<int>>& _Map, Node* tree, int n, int _size, float _rate)
{
    if (n == 1) 
    {
        if (_size > GetRoomSize(tree->nodeRect))
        {
            printf("%d, %d ~ %d, %d ERRor\n", tree->nodeRect.x, tree->nodeRect.y , tree->nodeRect.height, tree->nodeRect.width);
        }
        return; // ���̻� ���� ���� �ʿ䰡 ���� �� return;
    }
    //�� ���� ��쿡��

    const RectInt curRect = tree->nodeRect;
    int maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;;
    bool is_height = curRect.height > curRect.width ? true : false;

    int split = maxLength / 2;

    int leftnodecnt = n / 2;
    int rightnodecnt = n - leftnodecnt;

    int max_room_size = static_cast<int>((_size + 1) * 2 * _rate);


    RectInt LeftRect, RightRect;

    GetChildRect(curRect, split, is_height, LeftRect, RightRect);

    int leftSize = GetRoomSize(LeftRect);
    int rightSize = GetRoomSize(RightRect);
    if (leftSize == 0)
    {
        tree->rightNode = new Node(RightRect);
        tree->rightNode->parNode = tree;
        Divide(_Map, tree->rightNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
        return;
    }
    else if (rightSize == 0)
    {
        tree->leftNode = new Node(LeftRect);
        tree->leftNode->parNode = tree;
        Divide(_Map, tree->leftNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
        return;
    }


    if (n % 2 == 1)
    {
        int tmp = (maxLength * leftSize) / (leftSize + 2* rightSize);
		split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(min_rate, max_rate) */* 0.67f **/ (tmp));
    }
    else
    {
        int tmp = (maxLength * leftSize) / (leftSize + rightSize);
        split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(min_rate, max_rate) * (tmp));
    }

    //���� �� �ִ� �ִ� ���̿� �ּ� �����߿��� �������� �� ���� ����


	{
		int trycnt = 0;
		while (true)
		{
			trycnt++;
			if (trycnt > static_cast<int>(maxLength * max_rate))
			{
				break;
			}
            GetChildRect(curRect, split, is_height, LeftRect, RightRect);
            leftSize = GetRoomSize(LeftRect);
            rightSize = GetRoomSize(RightRect);
			if (max_room_size * leftnodecnt > leftSize && max_room_size * rightnodecnt > rightSize)
			{
				printf("ERRRRRRRR");
				return;
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
	

		// ���� �ּ� ũ�⸦ ���߱� �����̴�.

		tree->leftNode = new Node(LeftRect);
		//���� ��忡 ���� ������ 
		//��ġ�� ���� �ϴ� �����̹Ƿ� ������ ������, ���� ���̴� ������ ���� �������� �־��ش�.

		tree->rightNode = new Node(RightRect);
		//���� ��忡 ���� ������ 
		//��ġ�� ���� �ϴܿ��� ���������� ���� ���̸�ŭ �̵��� ��ġ�̸�, ���� ���̴� ���� ���α��̿��� ���� ���� ���ΰ��� �� ������ �κ��� �ȴ�.

		DrawLine(_Map, curRect, split, is_height, n);
		//�� �� �� �ΰ��� ��带 ������ ���� �׸��� �Լ��̴�.        
	}

    tree->leftNode->parNode = tree; //�ڽĳ����� �θ��带 �������� ���� ����
    tree->rightNode->parNode = tree;
    float nrate = _rate * Rate > 1.0 ? _rate * Rate : 1.0f;
	Divide(_Map, tree->leftNode, leftnodecnt, _size, nrate); //����, ������ �ڽ� ���鵵 �����ش�.
	Divide(_Map, tree->rightNode, rightnodecnt, _size, nrate);//����, ������ �ڽ� ���鵵 �����ش�.
}

void GenerateRoom::DrawLine(std::vector<std::vector<int>>& _Map, const RectInt& _cur, int splite, bool is_height, int n)
{
	if (is_height)
	{
        int mid = _cur.y + _cur.width / 2;
        int diff = static_cast<int>(_cur.width * 0.35f / n);
		int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - diff, mid + diff);
		for (int i = _cur.y; i < _cur.y + _cur.width; ++i)
		{
			if (0 == _Map[_cur.x + splite][i])
			{
				_Map[_cur.x + splite][i] = 1;
			}
		}

        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
        {
            if (_Map[_cur.x + splite][i] != -1)
            {
                _Map[_cur.x + splite][i] = 2;
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
			if (0 == _Map[i][_cur.y + splite])
			{
				_Map[i][_cur.y + splite] = 1;
			}
		}
        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
        {
            if (_Map[i][_cur.y + splite] != -1)
            {
                _Map[i][_cur.y + splite] = 2;
            }
        }
	}
}

// �� : 1, �� : 0, �� : 2, ���� ���� : -1 �� ǥ��, (���3??�����)


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
