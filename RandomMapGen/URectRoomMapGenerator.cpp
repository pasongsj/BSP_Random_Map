#include "URectRoomMapGenerator.h"

URectRoomMapGenerator::URectRoomMapGenerator() 
{
}

URectRoomMapGenerator::~URectRoomMapGenerator() 
{
}



bool URectRoomMapGenerator::CreateMap()
{
    URectMapGenerator::CreateMap();


    float ratio = static_cast<float>(GetRoomSize({ 1, 1, lx - 1, ly - 1 })) / ((min_room_size + 1) * 2 * room_cnt);

    // spare을 늘려가며 가능한 랜덤맵이 있는지 확인한다.
    while (true)
    {
        // 테스트할 맵을 복사한다
        CpyMap();

        // 방 크기의 여유 비율
        float _rate = ratio * spare + 0.1f < 1.0f ? 1.0f : ratio * spare + 0.1f;

        // 맵 생성
        if (nullptr == RootNode)
        {
            RootNode = std::make_shared<Node>();
            RootNode->nodeRect = { 1, 1, lx - 1, ly - 1 }; // 맵의 테두리를 제외한 크기의 루트노드
        }
        bool can_gen = DivideNode(RootNode, room_cnt, min_room_size, _rate);

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
    for (int i = 0; i < lx; ++i) // 랜덤맵 생성을 성공한 경우 cpy
    {
        for (int j = 0; j < ly; ++j)
        {
            base_map[i][j] = try_map_gen[i][j];
        }
    }

    for (std::shared_ptr<Node> _leaf : LeafNodeList)
    {
        CreateRoom(_leaf);
    }


    // 노드 release
    ReleaseNode(RootNode);
    RootNode = nullptr;

    return true;

}

// 현재 Node를 n개로 나누고싶다는 의미
bool URectRoomMapGenerator::DivideNode(std::shared_ptr<Node> tree, int n, int _size, float _rate)
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
        return DivideNode(tree->rightNode, n, _size, _rate);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
    else if (rightSize == 0)
    {
        tree->leftNode = std::make_shared<Node>();
        tree->leftNode->nodeRect = LeftRect;
        tree->leftNode->parNode = tree;
        return DivideNode(tree->leftNode, n, _size, _rate);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
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
        DrawLine(curRect, split, is_height, n);
    }

    tree->leftNode->parNode = tree; //자식노드들의 부모노드를 나누기전 노드로 설정
    tree->rightNode->parNode = tree;

    // 여분비율을 낮춘다
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;

    return DivideNode(tree->leftNode, leftnodecnt, _size, nrate) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate); //왼쪽, 오른쪽 자식 노드들도 나눠준다.

}


bool URectRoomMapGenerator::CreateMap(std::vector<std::vector<int>> _map, int _roomcnt, int _min_room_size, int _doorsize)
{
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;

    return URectRoomMapGenerator::CreateMap();
}

//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
void URectRoomMapGenerator::DrawLine(const RectInt& _cur, int splite, bool is_height, int n)
{
    if (is_height)
    {
        int mid = _cur.y + _cur.width / 2;
        int diff = static_cast<int>(_cur.width * 0.35f / n);
        int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - diff, mid + diff);
        for (int i = _cur.y; i < _cur.y + _cur.width; ++i)
        {
            if (0 == try_map_gen[_cur.x + splite][i])
            {
                try_map_gen[_cur.x + splite][i] = 1;
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
            if (0 == try_map_gen[i][_cur.y + splite])
            {
                try_map_gen[i][_cur.y + splite] = 1;
            }
        }
    }
}

void URectRoomMapGenerator::CreateRoom(std::shared_ptr<Node> _leafNode)
{
    RectInt CurRect = _leafNode->nodeRect;
    if (CurRect.width < 5 || CurRect.height < 5)
    {
        return;
    }


    int random_polygon = GameEngineRandom::MainRandom.RandomInt(0, 4);

    switch (random_polygon)
    {
    case 0:
    {
		// 삼각형
		if (CurRect.width >= 5 && CurRect.height >= 5)
		{
			int point_len = GameEngineRandom::MainRandom.RandomInt(0, CurRect.width - 2);
			int b_x = CurRect.x;
			int b_y = CurRect.y + point_len;


			int a_x = CurRect.x + CurRect.height - 2;
			int a_y = CurRect.y;

			int c_x = CurRect.x + CurRect.height - 2;
			int c_y = CurRect.y + CurRect.width - 2;

			if (base_map[b_x][b_y] != 0 || base_map[a_x][a_y] != 0 || base_map[c_x][c_y] != 0)
			{
				return;
			}

			for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
			{
				for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
				{
					if ((i * (b_y - a_y) < ((b_x - a_x) * (j - b_y) + b_x * (b_y - a_y))) || (i * (b_y - c_y) > ((b_x - c_x) * (j - b_y) + b_x * (b_y - c_y))))
					{
						if (base_map[i][j] == 0)
						{
							base_map[i][j] = 1;
						}
					}
				}
			}
		}
        break;
    }
    case 1:
    case 2:
    case 3:
    {

        // 사각형
        if (CurRect.width >= 5 && CurRect.height >= 5)
        {

            int start_x = GameEngineRandom::MainRandom.RandomInt(CurRect.x, CurRect.x + CurRect.height / 4);
            int start_y = GameEngineRandom::MainRandom.RandomInt(CurRect.y, CurRect.y + CurRect.width / 4);
            int len_x = GameEngineRandom::MainRandom.RandomInt(CurRect.height / 2, CurRect.x + CurRect.height- start_x); 
            int len_y = GameEngineRandom::MainRandom.RandomInt(CurRect.width / 2, CurRect.y + CurRect.width - start_y);


            for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
            {
                for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
                {
                    if (i < start_x || start_x >= start_x + len_x || j < start_y || j >= start_y + len_y)
                    {
                        if (base_map[i][j] == 0)
                        {
                            base_map[i][j] = 1;
                        }
                    }
                }
            }
        }
        break;

    }
    case 4:
    {
        // 마름모
        if (CurRect.width >= 3 && CurRect.height >= 3)
        {
            int point_x = (CurRect.height) / 2 - 1;
            int point_y = (CurRect.width) / 2 - 1;

            int a_x = CurRect.x + point_x;
            int a_y = CurRect.y;

            int b_x = CurRect.x;
            int b_y = CurRect.y + point_y;


            int c_x = CurRect.x + point_x;
            int c_y = CurRect.y + point_y * 2;


            int d_x = CurRect.x + point_x * 2;
            int d_y = CurRect.y + point_y;

            for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
            {
                for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
                {
                    if (
                        (i * (b_y - a_y) < ((b_x - a_x) * (j - b_y) + b_x * (b_y - a_y))) || (i * (b_y - c_y) > ((b_x - c_x) * (j - b_y) + b_x * (b_y - c_y))) ||
                        (i * (d_y - a_y) > ((d_x - a_x) * (j - d_y) + d_x * (d_y - a_y))) || (i * (d_y - c_y) < ((d_x - c_x) * (j - d_y) + d_x * (d_y - c_y)))
                        )
                    {
                        if (base_map[i][j] == 0)
                        {
                            base_map[i][j] = 1;
                        }
                    }
                }
            }
        }
        break;

    }
    default:
        break;
    }
    // // 삼각형
    //if (CurRect.width >= 5 && CurRect.height >= 5)
    //{
    //    int point_len = GameEngineRandom::MainRandom.RandomInt(0, CurRect.width-2);
    //    int b_x = CurRect.x;
    //    int b_y = CurRect.y + point_len;


    //    int a_x = CurRect.x + CurRect.height-2;
    //    int a_y = CurRect.y;

    //    int c_x = CurRect.x + CurRect.height-2;
    //    int c_y = CurRect.y + CurRect.width-2;

    //    if (base_map[b_x][b_y] != 0 || base_map[a_x][a_y] != 0 || base_map[c_x][c_y] != 0)
    //    {
    //        return;
    //    }

    //    for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
    //    {
    //        for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
    //        {
    //            if ((i * (b_y - a_y) < ((b_x - a_x) * (j - b_y) + b_x * (b_y - a_y))) || (i * (b_y - c_y) > ((b_x - c_x) * (j - b_y) + b_x * (b_y - c_y))))
    //            {
    //                if (base_map[i][j] == 0)
    //                {
    //                    base_map[i][j] = 1;
    //                }
    //            }
    //        }
    //    }
    //}

    //// 사각형
    //if (CurRect.width >= 5 && CurRect.height >= 5)
    //{

    //    int start_x = GameEngineRandom::MainRandom.RandomInt(CurRect.x, CurRect.x + CurRect.height / 4);
    //    int start_y = GameEngineRandom::MainRandom.RandomInt(CurRect.y, CurRect.y + CurRect.width / 4);
    //    int len_x = GameEngineRandom::MainRandom.RandomInt(CurRect.height / 2, CurRect.x + CurRect.height- start_x); 
    //    int len_y = GameEngineRandom::MainRandom.RandomInt(CurRect.width / 2, CurRect.y + CurRect.width - start_y);


    //    for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
    //    {
    //        for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
    //        {
    //            if (i < start_x || start_x >= start_x + len_x || j < start_y || j >= start_y + len_y)
    //            {
    //                if (base_map[i][j] == 0)
    //                {
    //                    base_map[i][j] = 1;
    //                }
    //            }
    //        }
    //    }
    //}

    //// 마름모
    //if (CurRect.width >= 3 && CurRect.height >= 3)
    //{
	//	int point_x = (CurRect.height) / 2 - 1;
	//	int point_y = (CurRect.width) / 2 - 1;
    //
    //    int a_x = CurRect.x + point_x;
    //    int a_y = CurRect.y;
    //
    //    int b_x = CurRect.x ;
    //    int b_y = CurRect.y + point_y;
    //
    //
    //    int c_x = CurRect.x + point_x;
	//	int c_y = CurRect.y + point_y * 2;
    //
    //
	//	int d_x = CurRect.x + point_x * 2;
    //    int d_y = CurRect.y + point_y;
    //
    //    for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
	//	{
	//		for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
	//		{
	//			if (
    //                (i * (b_y - a_y) < ((b_x - a_x) * (j - b_y) + b_x * (b_y - a_y))) || (i * (b_y - c_y) > ((b_x - c_x) * (j - b_y) + b_x * (b_y - c_y))) ||
    //                (i * (d_y - a_y) > ((d_x - a_x) * (j - d_y) + d_x * (d_y - a_y))) || (i * (d_y - c_y) < ((d_x - c_x) * (j - d_y) + d_x * (d_y - c_y))) 
    //                )
	//			{
	//				if (base_map[i][j] == 0)
	//				{
	//					base_map[i][j] = 1;
	//				}
	//			}
	//		}
	//	}
    //}
}
