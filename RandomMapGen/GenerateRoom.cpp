#include "GenerateRoom.h"
#include "GameEngineRandom.h"

#include <iostream>
#include <queue>
#include <tuple>


GenerateRoom* GenerateRoom::Room = nullptr;

float GenerateRoom::m_rate = 0.2f;
int GenerateRoom::door_size = 1;
float GenerateRoom::spare = 1.0f;
Node* GenerateRoom::RootNode = nullptr;


std::vector<std::vector<bool>> GenerateRoom::isvisited;
std::vector<std::vector<int>>  GenerateRoom::MapSizeIndex;
std::vector<std::vector<int>>  GenerateRoom::TryMap;

int GenerateRoom::lx = 0;
int GenerateRoom::ly = 0;



GenerateRoom::GenerateRoom() 
{
    Room = this;
}

GenerateRoom::~GenerateRoom() 
{
}

void GenerateRoom::ReleaseNode(Node* _cNode)
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

            if (_Map[i][j] == -1)// -1�� �κ��� �ִٸ� BFS�� ���� �ֺ��� ���� ����
            {
                SetWallBFS(i, j, _Map);
            }
			else if (0 == _Map[i][j] && (i == 0 || i == lx - 1 || j == 0 || j == ly - 1)) // �׵θ��κп��� ���� ����
            {
                _Map[i][j] = 1;
            }
            isvisited[i][j] = true;
        }
    }
    
    // �� ũ�⸦ ������ ����ϱ� ����
    CalMapSizeIndex(_Map, MapSizeIndex);

    // ��ü ���� / �ִ� ��ũ��+�׵θ� ����
    spare = 0.8f;
	float ratio = static_cast<float>(GetRoomSize({ 1, 1, lx - 1, ly - 1 })) / ((_size + 1) * 2 * roomcnt);


    // spare�� �÷����� ������ �������� �ִ��� Ȯ���Ѵ�.
    while(true)
    {
        // �׽�Ʈ�� ���� �����Ѵ�
        CpyMap(_Map);

        // �� ũ���� ���� ����
		float _rate = ratio * spare + 0.1f < 1.0f ? 1.0f : ratio * spare + 0.1f;

        // �� ����
        if (nullptr == RootNode)
        {
            RootNode = new Node({ 1, 1, lx - 1, ly - 1 }); // ���� �׵θ��� ������ ũ���� ��Ʈ���
        }
        bool can_gen = Divide(RootNode, roomcnt, _size, _rate);

        // ��� release
        ReleaseNode(RootNode);
        RootNode = nullptr;

        if (true == can_gen) // ������ ���� ����
        {
            break;
        }
\
        spare += 0.01f;
        if (spare >= 1.0f) // ������ ���� ����
        {
            printf("err");
            return;
        }
    }

    for (int i = 0; i < lx; ++i) // ������ ������ ������ ��� cpy
    {
        for (int j = 0; j < ly; ++j)
        {
            _Map[i][j] = TryMap[i][j];
        }
    }
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


bool GenerateRoom::Divide(Node* tree, int n, int _size, float _rate)
{
    if (n == 1) // ���̻� ���� ���� �ʿ䰡 ���� ��
    {
        if (_size > GetRoomSize(tree->nodeRect)) // �ּ� �� ũ�⸦ �������� �ʴ´ٸ�
        {
            return false;
        }
        return true; 
    }


    // ���� �� ������ �� ��

    // ���� ����� �� ����
    const RectInt curRect = tree->nodeRect;

    // ���η� ������ ���η� ������
    int maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;;
    bool is_height = curRect.height > curRect.width ? true : false;

    // 5:5 ������ �����ٰ� ����
    int split = maxLength / 2;

    // ���ʳ���� �� ����
    int leftnodecnt = n / 2;
    // ������ ����� �� ����
    int rightnodecnt = n - leftnodecnt;

    // �ִ� �� ũ�� * ���� ����
    int max_room_size = static_cast<int>((_size + 1) * 2 * _rate);


    // 5:5�� �������� �� Left�� Right�ӽð�
    RectInt LeftRect, RightRect;
    GetChildRect(curRect, split, is_height, LeftRect, RightRect);

    int leftSize = GetRoomSize(LeftRect);
    int rightSize = GetRoomSize(RightRect);

    // ������ ��� -1�ΰ�� ���� ���� �� ���� ������ �ݴ��� ��忡 n����ŭ �����Ѵ�.
    if (leftSize == 0)
    {
        tree->rightNode = new Node(RightRect);
        tree->rightNode->parNode = tree;
        return Divide(tree->rightNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
    }
    else if (rightSize == 0)
    {
        tree->leftNode = new Node(LeftRect);
        tree->leftNode->parNode = tree;
        return Divide(tree->leftNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
    }


    // Ȧ������ ���� �������ϴ� ��� 1 : 2 ũ�� ������ split�� ����ϱ� ����
    if (n % 2 == 1)
    {
        int tmp = (maxLength * leftSize) / (leftSize + 2* rightSize);
		split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (tmp));
    }
    else
    {
        int tmp = (maxLength * leftSize) / (leftSize + rightSize);
		split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(1.0f - m_rate, 1.0f + m_rate) * (tmp));
    }

    // ���� �� �ִ� �ִ� ���̿� �ּ� �����߿��� �������� �� ���� ����


	{
        // ���� ���� ������ ���߱� ���� split�� �����ϴ� ����
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
		// ���� �ּ� ũ�⸦ ���߱� �����̴�.

		//���� ��忡 ���� ������ 
		tree->leftNode = new Node(LeftRect);

		//���� ��忡 ���� ������ 
		tree->rightNode = new Node(RightRect);

		//�� �� �� �ΰ��� ��带 ������ ���� �׸��� �Լ��̴�.        
		DrawLine(curRect, split, is_height, n);
	}

    tree->leftNode->parNode = tree; //�ڽĳ����� �θ��带 �������� ���� ����
    tree->rightNode->parNode = tree;

    // ���к����� �����
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;

    return Divide(tree->leftNode, leftnodecnt, _size, nrate) && Divide(tree->rightNode, rightnodecnt, _size, nrate); //����, ������ �ڽ� ���鵵 �����ش�.
}

void GenerateRoom::DrawLine(const RectInt& _cur, int splite, bool is_height, int n)
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

        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
        {
            if (TryMap[_cur.x + splite][i] != -1)
            {
                TryMap[_cur.x + splite][i] = 2;
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
        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
        {
            if (TryMap[i][_cur.y + splite] != -1)
            {
                TryMap[i][_cur.y + splite] = 2;
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
