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



    // spare을 늘려가며 가능한 랜덤맵이 있는지 확인한다.
    while (true)
    {
        // 테스트할 맵을 복사한다
        CpyMap();


        // 맵 생성


		bool can_gen = true;
        if (CurMapShape == MapShape::none)
        {
            // 방 크기의 여유 비율
            float ratio = static_cast<float>(GetRoomSize({ 1, 1, lx - 2, ly - 2 })) / ((min_room_size + 1) * 2 * room_cnt);
            float _rate = ratio * spare + 0.1f < 1.0f ? 1.0f : ratio * spare + 0.1f;

			if (nullptr == RootNode)
			{
				RootNode = new Node();
				RootNode->nodeRect = { 1, 1, lx - 1, ly - 1 }; // 맵의 테두리를 제외한 크기의 루트노드
			}
			can_gen = DivideNode(RootNode, room_cnt, min_room_size, _rate, true);
        }
        else
        {
            
            int remain_room = room_cnt % ShapeList.size();
            for (Node* Roots : ShapeList)
            {
                // 방 크기의 여유 비율
                float ratio = static_cast<float>(GetRoomSize(Roots->nodeRect)) / ((min_room_size + 1) * 2 * (room_cnt / ShapeList.size() + 1));
                float _rate = ratio * spare + 0.1f < 1.0f ? 1.0f : ratio * spare + 0.1f;

                if (remain_room > 0)
                {
                    can_gen = can_gen && DivideNode(Roots, room_cnt / static_cast<int>(ShapeList.size()) + 1, min_room_size, _rate, true);
                    remain_room--;
                }
                else
                {
                    can_gen = can_gen && DivideNode(Roots, room_cnt / static_cast<int>(ShapeList.size()), min_room_size, _rate, true);
                }
            }
        }
       

        if (true == can_gen) // 랜덤맵 생성 성공
        {
            break;
        }
        else
        {
            // 노드 release
            LeafNodeList.clear();
            if (CurMapShape == MapShape::none)
            {
                ReleaseNode(RootNode);
                RootNode = nullptr;
            }
            else
            {
                for (Node* Roots : ShapeList)
                {
                    ReleaseNode(Roots->rightNode);
                    Roots->rightNode = nullptr;
                    ReleaseNode(Roots->leftNode);
                    Roots->leftNode = nullptr;
                }
            }
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



    switch (CurMapShape)
    {
    case MapShape::none:
        ConnectRoom(RootNode);
        break;
    case MapShape::giyeok:
        for (int i = 0; i < ShapeList.size() - 1; ++i)
        {
            ConnectRoom(ShapeList[i], ShapeList[i + 1]);
        }
        break;
    case MapShape::digeut:
        for (int i = 0; i < ShapeList.size() - 1; ++i)
        {
            ConnectRoom(ShapeList[i], ShapeList[i + 1]);
        }
        break;
    case MapShape::mieum:
        for (int i = 0; i < ShapeList.size() - 1; ++i)
        {
            ConnectRoom(ShapeList[i], ShapeList[i+1]);
        }
        ConnectRoom(ShapeList[ShapeList.size() - 1]);
        break;
    case MapShape::cross:
        for (int i = 1; i < 5; ++i)
        {
            ConnectRoom(ShapeList[i], ShapeList[0]);
        }
        ConnectRoom(ShapeList[0]);
        break;
    default:
        break;
    }

         //방 모양 만들기
    for (Node* _leaf : LeafNodeList)
    {
        CreateRoom(_leaf);
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

bool URectRoomMapGenerator::ConnectRoom(Node* main_node, Node* sub_node)
{
    if (nullptr == main_node)
    {
        return true;
    }
	RectInt f_rect;
	RectInt s_rect;
    if (sub_node != nullptr)
    {
        f_rect = main_node->nodeRect;
        s_rect = sub_node->nodeRect;
    }
    else
    {
        if (nullptr == main_node->leftNode || nullptr == main_node->rightNode)
        {
            return true;
        }
        f_rect = main_node->leftNode->nodeRect;
        s_rect = main_node->rightNode->nodeRect;
    }
    
    // x축(세로)가 공통인지 확인


    if ((f_rect.x <= s_rect.x && s_rect.x < f_rect.x + f_rect.height) || (f_rect.x >= s_rect.x && s_rect.x + s_rect.height > f_rect.x))
    {

        int midx = (std::max(f_rect.x, s_rect.x) + std::min(f_rect.x + f_rect.height, s_rect.x + s_rect.height)) / 2;
        int miny, maxy;


        if (s_rect.GetMidy() > f_rect.GetMidy())
        {
            miny = f_rect.GetMidy();
            maxy = s_rect.GetMidy() + 1;
        }
        else
        {
            miny = s_rect.GetMidy();
            maxy = f_rect.GetMidy() + 1;
        }
        for (int j = miny; j < maxy; ++j)
        {
			base_map[midx][j] = EMapGeneratorData::Passage;
        }
        if (sub_node != nullptr)
        {
            return ConnectRoom(main_node);
        }
        else
        {
            return ConnectRoom(main_node->leftNode)&&ConnectRoom( main_node->rightNode);
        }
    }
    // y 축(가로)가 공통인지 확인
    else if ((f_rect.y <= s_rect.y && s_rect.y < f_rect.y + f_rect.width) || (f_rect.y > s_rect.y && s_rect.y + s_rect.width > f_rect.y))
    {
        int midy = (std::max(f_rect.y, s_rect.y) + std::min(f_rect.y + f_rect.width, s_rect.y + s_rect.width)) / 2;
        int minx, maxx;


        if (s_rect.GetMidx() > f_rect.GetMidx())
        {
            minx = f_rect.GetMidx();
            maxx = s_rect.GetMidx() + 1;
        }
        else
        {
            minx = s_rect.GetMidx();
            maxx = f_rect.GetMidx() + 1;
        }
        for (int j = minx; j < maxx; ++j)
        {
			base_map[j][midy] = EMapGeneratorData::Passage;
        }
        if (sub_node != nullptr)
        {
            return ConnectRoom(main_node);
        }
        else
        {
            return ConnectRoom(main_node->leftNode) && ConnectRoom(main_node->rightNode);
        }
    }
    else
    {
        int a = 0;
        return false;
    }
}


// 현재 Node를 n개로 나누고싶다는 의미
bool URectRoomMapGenerator::DivideNode(Node* tree, int n, int _size, float _rate)
{
    if (n == 0)
    {
        FillTryMapRect(tree->nodeRect, EMapGeneratorData::Wall);
        return true;
    }
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
    int maxLength = curRect.height >= curRect.width ? curRect.height : curRect.width;;
    bool is_height = curRect.height >= curRect.width ? true : false;

    // 5:5 비율로 나눈다고 가정
	int split = (maxLength - 1) / 2;

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

    //// 한쪽이 모두 -1인경우 방을 만들 수 없기 때문에 반대쪽 노드에 n개만큼 생성한다.
    //if (leftSize == 0)
    //{
    //    tree->rightNode = new Node();
    //    tree->rightNode->nodeRect = RightRect;
    //    tree->rightNode->parNode = tree;
    //    return DivideNode(tree->rightNode, n, _size, _rate);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
    //}
    //else if (rightSize == 0)
    //{
    //    tree->leftNode = new Node();
    //    tree->leftNode->nodeRect = LeftRect;
    //    tree->leftNode->parNode = tree;
    //    return DivideNode(tree->leftNode, n, _size, _rate);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
    //}


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
            else if ((maxLength-1) / 2 == split)
            {
                split++;
            }
            else
            {
                break;

            }
        }
        // 방의 최소 크기를 맞추기 위함이다.

        //왼쪽 노드에 대한 정보다 
        tree->leftNode = new Node();
        tree->leftNode->nodeRect = LeftRect;

        //우측 노드에 대한 정보다 
        tree->rightNode = new Node();
        tree->rightNode->nodeRect = RightRect;

        //그 후 위 두개의 노드를 나눠준 선을 그리는 함수이다.        
        DrawLine(curRect, split, is_height, n);
    }

    tree->leftNode->parNode = tree; //자식노드들의 부모노드를 나누기전 노드로 설정
    tree->rightNode->parNode = tree;

    // 여분비율을 낮춘다
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;
    if (true == is_height)
    {
        return DivideNode(tree->leftNode, leftnodecnt, _size, nrate) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
    else
    {
        return DivideNode(tree->rightNode, rightnodecnt, _size, nrate) && DivideNode(tree->leftNode, leftnodecnt, _size, nrate); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }

}

bool URectRoomMapGenerator::DivideNode(Node* tree, int n, int _size, float _rate, bool is_reverse)
{
    if (n == 0)
    {
        return true;
    }
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
        tree->rightNode = new Node();
        tree->rightNode->nodeRect = RightRect;
        tree->rightNode->parNode = tree;
        return DivideNode(tree->rightNode, n, _size, _rate);//왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
    else if (rightSize == 0)
    {
        tree->leftNode = new Node();
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
        tree->leftNode = new Node();
        tree->leftNode->nodeRect = LeftRect;

        //우측 노드에 대한 정보다 
        tree->rightNode = new Node();
        tree->rightNode->nodeRect = RightRect;

        //그 후 위 두개의 노드를 나눠준 선을 그리는 함수이다.        
        DrawLine(curRect, split, is_height, n);
    }

    tree->leftNode->parNode = tree; //자식노드들의 부모노드를 나누기전 노드로 설정
    tree->rightNode->parNode = tree;

    // 여분비율을 낮춘다
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;
    if (true == is_reverse) // 역순
    {
        return DivideNode(tree->rightNode, rightnodecnt, _size, nrate, true) && DivideNode(tree->leftNode, leftnodecnt, _size, nrate, false); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
    else // 정순
    {
        return DivideNode(tree->leftNode, leftnodecnt, _size, nrate, true) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate, false); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
}




bool URectRoomMapGenerator::CreateMap(std::vector<std::vector<EMapGeneratorData>> _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape)
{
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;
    CurMapShape = _Shape;
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
            if (EMapGeneratorData::Ground == try_map_gen[_cur.x + splite][i])
            {
                try_map_gen[_cur.x + splite][i] = EMapGeneratorData::Wall;
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
            if (EMapGeneratorData::Ground == try_map_gen[i][_cur.y + splite])
            {
                try_map_gen[i][_cur.y + splite] = EMapGeneratorData::Wall;
            }
        }
    }
}
//
//bool URectRoomMapGenerator::MakeRoad(const RectInt f_rect, const RectInt s_rect)
//{
//    // x축(세로)가 공통인지 확인
//    if ((f_rect.x <= s_rect.x && s_rect.x < f_rect.x + f_rect.height) || (f_rect.x >= s_rect.x && s_rect.x + s_rect.height > f_rect.x))
//    {
//
//        int midx = (std::max(f_rect.x, s_rect.x) + std::min(f_rect.x + f_rect.height, s_rect.x + s_rect.height)) / 2;
//        int miny, maxy;
//
// 
//        if (s_rect.GetMidy() > f_rect.GetMidy())
//        {
//            miny = f_rect.GetMidy()-1;
//			maxy = s_rect.GetMidy() + 1;
//        }
//        else
//        {
//            maxy = f_rect.GetMidy()+1;
//            miny = s_rect.GetMidy()-1;
//        }
//        for (int j = miny; j < maxy; ++j)
//        {
//            if (EMapGeneratorData::Wall == base_map[midx][j])
//            {
//                if (EMapGeneratorData::Ground == base_map[midx][j - 1] || EMapGeneratorData::Ground == base_map[midx][j + 1])
//                {
//                    base_map[midx][j] = EMapGeneratorData::Door;
//                }
//                else
//                {
//                    base_map[midx][j] = EMapGeneratorData::Passage;
//                }
//            }
//        }
//        return true;
//    }
//    // y 축(가로)가 공통인지 확인
//    else if ((f_rect.y <= s_rect.y && s_rect.y < f_rect.y + f_rect.width) || (f_rect.y > s_rect.y && s_rect.y + s_rect.width > f_rect.y))
//    {
//        int midy = (std::max(f_rect.y, s_rect.y) + std::min(f_rect.y + f_rect.width, s_rect.y + s_rect.width)) / 2;
//        int minx, maxx;
//
//
//        if (s_rect.GetMidx() > f_rect.GetMidx())
//        {
//			minx = f_rect.GetMidx() - 1;
//			maxx = s_rect.GetMidx() + 1;
//        }
//        else
//        {
//			maxx = f_rect.GetMidx() + 1;
//			minx = s_rect.GetMidx() - 1;
//        }
//        for (int j = minx; j < maxx; ++j)
//        {
//            if (EMapGeneratorData::Wall == base_map[j][midy])
//            {
//				if (EMapGeneratorData::Ground == base_map[j - 1][midy] || EMapGeneratorData::Ground == base_map[j + 1][midy])
//                {
//                    base_map[j][midy] = EMapGeneratorData::Door;
//                }
//                else
//                {
//                    base_map[j][midy] = EMapGeneratorData::Passage;
//                }
//            }
//        }
//        return true;
//    }
//    else
//    {
//        int a = 0;
//        return false;
//        // err
//    }
//}


void URectRoomMapGenerator::CreateRoom(Node* _leafNode)
{
    RectInt CurRect = _leafNode->nodeRect;
    if (CurRect.width < 4 || CurRect.height < 4)
    {
        return;
    }

    int random_polygon = GameEngineRandom::MainRandom.RandomInt(0, 5);

	const int dx[4] = { 0,0,1,-1 };
	const int dy[4] = { 1,-1,0,0 };

    switch (random_polygon)
    {
    case 0:
    {
		// 삼각형
		//if (CurRect.width >= 4 && CurRect.height >= 4)
		{
			int point_len = GameEngineRandom::MainRandom.RandomInt(0, CurRect.width - 1);
			int b_x = CurRect.x;
			int b_y = CurRect.y + point_len;


			int a_x = CurRect.x + CurRect.height - 1;
			int a_y = CurRect.y;

			int c_x = CurRect.x + CurRect.height - 1;
			int c_y = CurRect.y + CurRect.width - 1;

			if (EMapGeneratorData::Ground != base_map[b_x][b_y] || EMapGeneratorData::Ground != base_map[a_x][a_y] || EMapGeneratorData::Ground != base_map[c_x][c_y])
			{
				return;
			}

			for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
			{
				for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
				{
                    if (i < c_x && (i * (b_y - a_y) > ((b_x - a_x) * (j - b_y) + b_x * (b_y - a_y))) && (i * (b_y - c_y) < ((b_x - c_x) * (j - b_y) + b_x * (b_y - c_y))))
                    {
                        base_map[i][j] = EMapGeneratorData::Ground;
                    }
                    else
                    {
                        if (EMapGeneratorData::Passage == base_map[i][j])
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                int nx = i + dx[k], ny = j + dy[k];
                                if (nx < c_x && (nx * (b_y - a_y) > ((b_x - a_x) * (ny - b_y) + b_x * (b_y - a_y))) && (nx * (b_y - c_y) < ((b_x - c_x) * (ny - b_y) + b_x * (b_y - c_y))))
                                {
                                    base_map[i][j] = EMapGeneratorData::Door;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            base_map[i][j] = EMapGeneratorData::Wall;
                        }
                    }
				}
			}
		}
        break;
    }
    case 1:
    case 2:
    {

        // 사각형
        //if (CurRect.width >= 3 && CurRect.height >= 3)
        {

			int start_x = GameEngineRandom::MainRandom.RandomInt(CurRect.x + 1, CurRect.x + CurRect.height / 5);
			int start_y = GameEngineRandom::MainRandom.RandomInt(CurRect.y + 1, CurRect.y + CurRect.width / 5);

			int len_x = GameEngineRandom::MainRandom.RandomInt((CurRect.x + CurRect.height - start_x - 1) * 4 / 5, CurRect.x + CurRect.height - start_x - 1);
			int len_y = GameEngineRandom::MainRandom.RandomInt((CurRect.y + CurRect.width - start_y - 1) * 4 / 5, CurRect.y + CurRect.width - start_y - 1);


            for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
            {
                for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
                {
                    if ((i > start_x && i < start_x + len_x) && (j > start_y && j < start_y + len_y))
                    {
                        base_map[i][j] = EMapGeneratorData::Ground;
                    }
                    else
                    {
                        if (EMapGeneratorData::Passage == base_map[i][j])
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                int nx = i + dx[k], ny = j + dy[k];
                                if ((nx > start_x && nx < start_x + len_x) && (ny > start_y && ny < start_y + len_y))
                                {
                                    base_map[i][j] = EMapGeneratorData::Door;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            base_map[i][j] = EMapGeneratorData::Wall;
                        }
                    }
                }
            }
        }
        break;

    }
    case 3:
    case 4:
    {
        // 마름모
        {
			int point_x = (CurRect.height - 1) / 2;
			int point_y = (CurRect.width - 1) / 2;

            int a_x = CurRect.x + point_x;
            int a_y = CurRect.y;

            int b_x = CurRect.x;
            int b_y = CurRect.y + point_y;


            int c_x = CurRect.x + point_x;
            int c_y = CurRect.y + point_y * 2;


            int d_x = CurRect.x + point_x * 2;
            int d_y = CurRect.y + point_y;

            for (int i = CurRect.x-1; i < CurRect.x + CurRect.height; ++i)
            {
                for (int j = CurRect.y-1; j < CurRect.y + CurRect.width; ++j)
                {
                    //| x - centerX | / a + | y - centerY | / b <= 1 마름모 내부
                    if (abs(i - (CurRect.x + point_x)) * point_y + abs(j - (CurRect.y + point_y)) * point_x <= point_x * point_y)
                    {
                        base_map[i][j] = EMapGeneratorData::Ground;
                    }
                    else
                    {
                        if (EMapGeneratorData::Passage == base_map[i][j])
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                int nx = i + dx[k], ny = j + dy[k];
                                if (abs(nx - (CurRect.x + point_x)) * point_y + abs(ny - (CurRect.y + point_y)) * point_x <= point_x * point_y)
                                {
                                    base_map[i][j] = EMapGeneratorData::Door;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            base_map[i][j] = EMapGeneratorData::Wall;
                        }
                    }
                }
            }
        }
        break;

    }
    case 5:
    {
        //circle
		int mid_x = CurRect.x + (CurRect.height - 1) / 2;
		int mid_y = CurRect.y + (CurRect.width - 1) / 2;
        int radius = CurRect.height > CurRect.width ? CurRect.width / 2 : CurRect.height / 2;
        for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
        {
            for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
            {

                if ((i - mid_x) * (i - mid_x) + (j - mid_y) * (j - mid_y) < radius * radius)
                {
                    base_map[i][j] = EMapGeneratorData::Ground;
                }
                else
                {
                    if (EMapGeneratorData::Passage == base_map[i][j])
                    {
                        for (int k = 0; k < 4; ++k)
                        {
                            int nx = i + dx[k], ny = j + dy[k];
                            if ((nx - mid_x) * (nx - mid_x) + (ny - mid_y) * (ny - mid_y) < radius * radius)
                            {
                                base_map[i][j] = EMapGeneratorData::Door;
                                break;
                            }
                        }
                    }
                    else
                    {
                        base_map[i][j] = EMapGeneratorData::Wall;
                    }
                }
            }
        }
    }
    default:
        break;
    }
}
