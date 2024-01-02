#include "GenerateRoom.h"
#include "GameEngineRandom.h"

#include <iostream>
#include <queue>
#include <tuple>


GenerateRoom* GenerateRoom::Room = nullptr;

float GenerateRoom::min_rate = 0.4f;
float GenerateRoom::max_rate = 0.6f;
int GenerateRoom::maxdepth = 0;
std::vector<std::vector<int>> GenerateRoom::Map;


GenerateRoom::GenerateRoom() 
{
    Room = this;
}

GenerateRoom::~GenerateRoom() 
{
}

bool GenerateRoom::In_range(int x, int y,int n, int m)
{
    return 0 < x && x < n && 0 < y && y < m;
}


void GenerateRoom::SetWallDFS(int x, int y, std::vector<std::vector<bool>>& _isvisited)
{
    std::queue<std::pair<int, int>> dfsq;
    dfsq.push(std::make_pair(x, y));
    _isvisited[x][y] = true;
	const int dx[4] = { 0,0,1,-1 };
	const int dy[4] = { 1,-1,0,0 };
    int n = _isvisited.size()-1;
    int m = _isvisited[0].size()-1;
    while (!dfsq.empty())
    {
        int cx, cy;
        std::tie(cx, cy) = dfsq.front();
        dfsq.pop();

        for (int i = 0; i < 4; ++i)
        {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (true == In_range(nx, ny, n, m))
            {
                if (0 == Map[nx][ny])
                {
                    Map[nx][ny] = 1;
                }
                else if (-1 == Map[nx][ny] && false == _isvisited[nx][ny])
                {
                    dfsq.push(std::make_pair(nx, ny));
                    _isvisited[nx][ny] = true;
                }
            }
        }

    }
}


void GenerateRoom::CreateMap(std::vector<std::vector<int>> _Map, int _maxdepth)
{
    Map = _Map;
    maxdepth = _maxdepth;
    int lx = Map.size();
    int ly = Map[0].size();

    std::vector<std::vector<bool>> isvisited(lx, std::vector<bool>(ly, false));

    for (int i = 0; i < lx; ++i)
    {
        for (int j = 0; j < ly; ++j)
        {
            if (true == isvisited[i][j])
            {
                continue;
            }
            if (Map[i][j] == -1)
            {
                SetWallDFS(i, j, isvisited);
            }
			else if (0 == Map[i][j] && (i == 0 || i == lx - 1 || j == 0 || j == ly - 1))
            {
                Map[i][j] = 1;
            }
            isvisited[i][j] = true;
        }
    }

    Node* RootNode = new Node({1, 1, static_cast<int>(Map.size()-1), static_cast<int>(Map[0].size()-1)});
    Divide(RootNode, _maxdepth);
}


void GenerateRoom::Divide(Node* tree, int n)
{
    if (n == 1) return; //내가 원하는 높이에 도달하면 더 나눠주지 않는다.
    //그 외의 경우에는

    //int maxLength = Mathf.Max(tree.nodeRect.width, tree.nodeRect.height);
    int maxLength = tree->nodeRect.width > tree->nodeRect.height ? tree->nodeRect.width : tree->nodeRect.height;
    //가로와 세로중 더 긴것을 구한후, 가로가 길다면 위 좌, 우로 세로가 더 길다면 위, 아래로 나눠주게 될 것이다.

    //int split = Mathf.RoundToInt(Random.Range(maxLength * minimumDevideRate, maxLength * maximumDivideRate));
    int split;
    if (n % 2 == 1)
    {
		split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(min_rate, max_rate) * maxLength * 0.67f);
    }
    else
    {

		split = static_cast<int>(GameEngineRandom::MainRandom.RandomFloat(min_rate, max_rate) * maxLength);
    }
    //나올 수 있는 최대 길이와 최소 길이중에서 랜덤으로 한 값을 선택

    RectInt curRect = tree->nodeRect;

    if (curRect.width >= curRect.height) //가로가 더 길었던 경우에는 좌 우로 나누게 될 것이며, 이 경우에는 세로 길이는 변하지 않는다.
    {
        tree->leftNode = new Node({ curRect.x, curRect.y, split, curRect.height});
        //왼쪽 노드에 대한 정보다 
        //위치는 좌측 하단 기준이므로 변하지 않으며, 가로 길이는 위에서 구한 랜덤값을 넣어준다.

        tree->rightNode = new Node({ curRect.x + split, curRect.y, curRect.width - split, curRect.height });
        //우측 노드에 대한 정보다 
        //위치는 좌측 하단에서 오른쪽으로 가로 길이만큼 이동한 위치이며, 가로 길이는 기존 가로길이에서 새로 구한 가로값을 뺀 나머지 부분이 된다.

        DrawLine({ curRect.x + split, curRect.y }, { curRect.x + split, curRect.y + curRect.height },n);
        //그 후 위 두개의 노드를 나눠준 선을 그리는 함수이다.        
    }
    else
    {
        tree->leftNode = new Node({ curRect.x, curRect.y, curRect.width, split });
        tree->rightNode = new Node({ curRect.x, curRect.y + split, curRect.width, curRect.height - split });
		DrawLine({ curRect.x, curRect.y + split }, { curRect.x + curRect.width, curRect.y + split }, n);
        //세로가 더 길었던 경우이다. 자세한 사항은 가로와 같다.
    }
    tree->leftNode->parNode = tree; //자식노드들의 부모노드를 나누기전 노드로 설정
    tree->rightNode->parNode = tree;
	Divide(tree->leftNode, n/2); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
	Divide(tree->rightNode, n -(n / 2));//왼쪽, 오른쪽 자식 노드들도 나눠준다.
}


// 벽 : 1, 땅 : 0, 문 : 2, 없는 지역 : -1 로 표기, (통로3??고민중)
void GenerateRoom::DrawLine(Vector2 from, Vector2 to,int n) //from->to로 이어지는 선을 그리게 될 것이다.
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
            if (0 == Map[from.x][starty])
            {
				Map[from.x][starty] = 1;
            }
        }
        Map[from.x][doorPos] = 2;
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
            if (0 == Map[startx][from.y])
            {
                Map[startx][from.y] = 1;
            }
        }
        Map[doorPos][from.y] = 2;
    }


}

void GenerateRoom::Print()
{
    for (int i = 0; i < 60; ++i)
    {
        for (int j = 0; j < 50; ++j)
        {
            std::cout << Map[i][j] << ' ';

        }
        std::cout << '\n';
    }
    Map.clear();
}
