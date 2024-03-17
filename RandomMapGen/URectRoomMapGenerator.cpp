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
				RootNode->nodeRect = { 1, 1, lx - 2, ly - 2 }; // 맵의 테두리를 제외한 크기의 루트노드
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

    //통로 연결하기
    bool can_connect = true;
    switch (CurMapShape)
    {
    case MapShape::none:
        can_connect = ConnectRoom(RootNode->leftNode,RootNode->rightNode);
        break;
    case MapShape::giyeok:
        for (int i = 0; i < ShapeList.size() - 1; ++i)
        {
			can_connect = can_connect && ConnectRoom(ShapeList[i], ShapeList[i + 1]);
        }
        break;
    case MapShape::digeut:
        for (int i = 0; i < ShapeList.size() - 1; ++i)
        {
            can_connect = can_connect && ConnectRoom(ShapeList[i], ShapeList[i + 1]);
        }
        break;
    case MapShape::mieum:
        for (int i = 0; i < ShapeList.size() - 1; ++i)
        {
            can_connect = can_connect && ConnectRoom(ShapeList[i], ShapeList[i+1]);
        }
        //ConnectRoom(ShapeList[ShapeList.size() - 1]);
        break;
    case MapShape::cross:
        for (int i = 1; i < 5; ++i)
        {
            can_connect = can_connect && ConnectRoom(ShapeList[i], ShapeList[0]);
        }
        //ConnectRoom(ShapeList[0]);
        break;
    default:
        break;
    }

    if (false == can_connect)
    {
        LeafNodeList.clear();
        ReleaseNode(RootNode);
        for (Node* _node : ShapeList)
        {
            ReleaseNode(_node);
        }
        RootNode = nullptr;
        return false;
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

bool URectRoomMapGenerator::ConnectRoom(Node* main_node, Node* sub_node) // 1 과 2를 연결함, 2는 nullptr일 수도 있음
{
    // door 계산용 임시 벡터
    std::vector<int> candoorvec;
    candoorvec.clear();
    if (nullptr == main_node || nullptr == sub_node)
    {
        return true;
    }
	RectInt f_rect = main_node->nodeRect;
    RectInt s_rect = sub_node->nodeRect;

    const int dx[8] = { 0,0,1,-1 , -1, -1, 1,1 };
    const int dy[8] = { 1,-1,0,0 , -1, 1, -1,1 };
    
    // x축(세로)가 공통인지 확인
    if (f_rect.x == s_rect.x)
    {
        int tmpy = f_rect.y > s_rect.y ? f_rect.y - 1 : s_rect.y - 1;
        for (int i = f_rect.x; i < f_rect.x + f_rect.height; ++i)
        {

            if (base_map[i][tmpy] != EMapGeneratorData::VoidTile)
            {
                bool can_passage = true;
                for (int k = 0; k < 8; ++k)
                {
                    int nx = i + dx[k], ny = tmpy + dy[k];
                    if (false == InRange(nx, ny) || EMapGeneratorData::VoidTile == base_map[nx][ny])
                    {
                        can_passage = false;
                        break;
                    }
                }
                if(true == can_passage)
                {
                    candoorvec.push_back(i);
                }
            }
        }
        int candoorvec_cnt = candoorvec.size() - door_size;
        if (candoorvec_cnt < 0)
        {
            return false;
        }
        int midx = candoorvec[GameEngineRandom::MainRandom.RandomInt(0, candoorvec_cnt)];
        int miny = std::min(f_rect.GetMidy(), s_rect.GetMidy());
        int maxy = std::max(f_rect.GetMidy(), s_rect.GetMidy());
        for (int j = miny; j < maxy; ++j)
        {
            for (int d = 0; d < door_size; ++d)
            {
				if (EMapGeneratorData::VoidTile != base_map[midx + d][j])
				{
					bool can_passage = true;
					for (int k = 0; k < 8; ++k)
					{
						int nx = midx + d + dx[k], ny = j + dy[k];
						if (false == InRange(nx, ny) || EMapGeneratorData::VoidTile == base_map[nx][ny])
						{
							can_passage = false;
							break;
						}
					}
					if (true == can_passage)
					{
						base_map[midx + d][j] = EMapGeneratorData::Passage;
					}
				}
            }

        }
    }
    else if (f_rect.y == s_rect.y)
    {
        std::vector<int> candoorvec;
        int tmpx = f_rect.x > s_rect.x ? f_rect.x - 1 : s_rect.x - 1;
        for (int i = f_rect.y; i < f_rect.y + f_rect.width; ++i)
        {
            if (base_map[tmpx][i] != EMapGeneratorData::VoidTile)
            {
                bool can_passage = true;
                for (int k = 0; k < 8; ++k)
                {
                    int nx = tmpx + dx[k], ny = i + dy[k];
                    if (false == InRange(nx, ny) || EMapGeneratorData::VoidTile == base_map[nx][ny])
                    {
                        can_passage = false;
                        break;
                    }
                }
                if (true == can_passage)
                {
                    candoorvec.push_back(i);
                }
            }
        }
        int candoorvec_cnt = candoorvec.size() - door_size;
        if (candoorvec_cnt < 0)
        {
            return false;
        }
        int midy = candoorvec[GameEngineRandom::MainRandom.RandomInt(0, candoorvec_cnt)];
        int minx = std::min(f_rect.GetMidx(), s_rect.GetMidx());
        int maxx = std::max(f_rect.GetMidx(), s_rect.GetMidx());
        for (int j = minx; j < maxx; ++j)
        {
            for (int d = 0; d < door_size; ++d)
            {
                if (EMapGeneratorData::VoidTile != base_map[j][midy+d])
                {
                    bool can_passage = true;
                    for (int k = 0; k < 8; ++k)
                    {
                        int nx = j + dx[k], ny = midy + d + dy[k];
                        if (false == InRange(nx, ny) || EMapGeneratorData::VoidTile == base_map[nx][ny])
                        {
                            can_passage = false;
                            break;
                        }
                    }
                    if (true == can_passage)
                    {
                        base_map[j][midy + d] = EMapGeneratorData::Passage;
                    }
                }
            }
        }
    }
    else
    {
        int a = 0;
        return false;
    }

    if (nullptr != main_node->rightNode)
    {
        if (nullptr != main_node->leftNode)
        {
            if (false == ConnectRoom(main_node->leftNode, main_node->rightNode))
            {
                return false;
            }
        }
        else 
        {
            if (false == ConnectRoom(main_node, main_node->rightNode))
            {
                return false;
            }
        }
    }
    if (nullptr != sub_node->rightNode)
    {
        if (nullptr != sub_node->leftNode)
        {
            if (false == ConnectRoom(sub_node->leftNode, sub_node->rightNode))
            {
                return false;
            }
        }
        else
        {
            if (false == ConnectRoom(sub_node, sub_node->rightNode))
            {
                return false;
            }
        }
    }
    return true;
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
    RectInt curRect = tree->nodeRect;

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
        curRect = RightRect;
        maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;;
        is_height = curRect.height > curRect.width ? true : false;

        // 5:5 비율로 나눈다고 가정
        split = maxLength / 2;

        // 왼쪽노드의 방 개수
        leftnodecnt = n / 2;
        // 오른쪽 노드의 방 개수
        rightnodecnt = n - leftnodecnt;

        // 최대 방 크기 * 여분 비율
        max_room_size = static_cast<int>((_size + 1) * 2 * _rate);


        // 5:5로 나누었을 때 Left와 Right임시값
        GetChildRect(curRect, split, is_height, LeftRect, RightRect);

        leftSize = GetRoomSize(LeftRect);
        rightSize = GetRoomSize(RightRect);
    }
    else if (rightSize == 0)
    {
        curRect = LeftRect;
        maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;;
        is_height = curRect.height > curRect.width ? true : false;

        // 5:5 비율로 나눈다고 가정
        split = maxLength / 2;

        // 왼쪽노드의 방 개수
        leftnodecnt = n / 2;
        // 오른쪽 노드의 방 개수
        rightnodecnt = n - leftnodecnt;

        // 최대 방 크기 * 여분 비율
        max_room_size = static_cast<int>((_size + 1) * 2 * _rate);


        // 5:5로 나누었을 때 Left와 Right임시값
        GetChildRect(curRect, split, is_height, LeftRect, RightRect);

        leftSize = GetRoomSize(LeftRect);
        rightSize = GetRoomSize(RightRect);
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
                return false;
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
                int lim = split + 4 < maxLength - 2 ? split + 4 : maxLength - 2;
                if (split + 1 > lim)
                {
                    return false;
                }
                split = GameEngineRandom::MainRandom.RandomInt(split + 1, lim);
            }
            else if (max_room_size * rightnodecnt > rightSize)
            {
                int lim = split - 4 > 2 ? split - 4 : 2;
                if (lim > split - 1)
                {
                    return false;
                }
                split = GameEngineRandom::MainRandom.RandomInt(lim, split - 1);
            }
            else
            {
                if (
                    (
                        true == is_height &&
                        (
                            EMapGeneratorData::Door == try_map_gen[curRect.x + split][curRect.y + curRect.width] ||
                            EMapGeneratorData::Door == try_map_gen[curRect.x + split][curRect.y - 1]
                            )
                        )
                    ||
                    (
                        false == is_height &&
                        (
                            EMapGeneratorData::Door == try_map_gen[curRect.x + curRect.height][curRect.y + split] ||
                            EMapGeneratorData::Door == try_map_gen[curRect.x - 1][curRect.y + split])
                        )
                    )
                {
                    // 홀수개의 방을 만들어야하는 경우 1 : 2 크기 비율로 split를 계산하기 위함
                    m_rate += 0.02f;
                    float left_lim, right_lim;
                    if (split < maxLength / 2)
                    {
                        right_lim = static_cast<float>(split - 1);
                        left_lim = right_lim * (1.0f - m_rate) > 2 ? right_lim * (1.0f - m_rate) : 2;
                    }
                    else
                    {
                        left_lim = static_cast<float>(split + 1);
                        right_lim = left_lim * (1.0f + m_rate) < maxLength - 2 ? left_lim * (1.0f + m_rate) : maxLength - 2;
                    }
                    if (left_lim >= right_lim)
                    {
                        return false;
                    }
                    split = GameEngineRandom::MainRandom.RandomInt(static_cast<int>(left_lim), static_cast<int>(right_lim));
                    continue;
                }
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
    if (true == is_reverse && CurMapShape!= MapShape::none) // 역순
    {
        return DivideNode(tree->rightNode, rightnodecnt, _size, nrate, true) && DivideNode(tree->leftNode, leftnodecnt, _size, nrate, false); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
    else // 정순
    {
        return DivideNode(tree->leftNode, leftnodecnt, _size, nrate, true) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate, false); //왼쪽, 오른쪽 자식 노드들도 나눠준다.
    }
}




bool URectRoomMapGenerator::CreateMap(const std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape)
{
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;
    CurMapShape = _Shape;

    const RoomType TypeList[4] = { RoomType::Triangle, RoomType::Rect, RoomType::Rhombus, RoomType::Circle };
    for (RoomType _Type : TypeList)
    {
        if (IgnoreRoomType.end() == IgnoreRoomType.find(_Type))
        {
            RoomTypeList.push_back(_Type);
        }
    }

    if (true == URectRoomMapGenerator::CreateMap())
    {
        //for (int i = 0; i < lx; ++i)
        //{
        //    for (int j = 0; j < ly; ++j)
        //    {
        //        _map[i][j] = base_map[i][j];
        //    }
        //}
        return true;
    }
    return false;
}

//자식 노드를 만들고 구분선을 그리는 함수 _cur 사각형에 대한 splite구분선이다
bool URectRoomMapGenerator::DrawLine(const RectInt& _cur, int splite, bool is_height, int n)
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
    return true;
}



void URectRoomMapGenerator::CreateRoom(Node* _leafNode)
{
    RectInt CurRect = _leafNode->nodeRect;


    RoomType RandomRoomType = RoomTypeList[GameEngineRandom::MainRandom.RandomInt(0, RoomTypeList.size() - 1)];

    bool clean_room = false;
    if (CurRect.width < 4 || CurRect.height < 4 || GetRoomSize(_leafNode->nodeRect) != ((CurRect.width) * (CurRect.height)))
    {
        RandomRoomType = RoomType::None;
        //random_polygon = 6;
        clean_room = true;
    }
	const int dx[4] = { 0,0,1,-1 };
	const int dy[4] = { 1,-1,0,0 };


    switch (RandomRoomType)
    {
    case URectRoomMapGenerator::RoomType::Rect:
    {

        // 사각형
        {

            int start_x = GameEngineRandom::MainRandom.RandomInt(CurRect.x , CurRect.x + CurRect.height / 4);
            int start_y = GameEngineRandom::MainRandom.RandomInt(CurRect.y , CurRect.y + CurRect.width / 4);

			int len_x = GameEngineRandom::MainRandom.RandomInt((CurRect.x + CurRect.height - start_x ) * 4 / 5, CurRect.x + CurRect.height - start_x);
			int len_y = GameEngineRandom::MainRandom.RandomInt((CurRect.y + CurRect.width - start_y ) * 4 / 5, CurRect.y + CurRect.width - start_y);

			if (len_x < 4 || len_y < 4 || (len_x * len_y < min_room_size))
            {
                clean_room = true;
                break;
            }

            for (int i = CurRect.x - 1; i <= CurRect.x + CurRect.height; ++i)
            {
                for (int j = CurRect.y - 1; j <= CurRect.y + CurRect.width; ++j)
                {
                    if ((i >= start_x && i < start_x + len_x) && (j >= start_y && j < start_y + len_y))
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
                                if ((nx >= start_x && nx < start_x + len_x) && (ny >= start_y && ny < start_y + len_y))
                                {
                                    base_map[i][j] = EMapGeneratorData::Door;
                                    break;
                                }
                            }
                        }
                        else if (EMapGeneratorData::Door == base_map[i][j])
                        {
                            continue;
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
    case URectRoomMapGenerator::RoomType::Triangle:
    {
        {
            int point_len = GameEngineRandom::MainRandom.RandomInt(0, CurRect.width - 1);
            int b_x = CurRect.x;
            int b_y = CurRect.y + point_len;


            int a_x = CurRect.x + CurRect.height - 1;
            int a_y = CurRect.y;

            int c_x = CurRect.x + CurRect.height - 1;
            int c_y = CurRect.y + CurRect.width - 1;

            if (EMapGeneratorData::Ground != base_map[b_x][b_y] || EMapGeneratorData::Ground != base_map[a_x][a_y] || EMapGeneratorData::Ground != base_map[c_x][c_y]||
				CurRect.height * CurRect.width / 2 < min_room_size)
            {
                clean_room = true;
                break;
            }

            for (int i = CurRect.x - 1; i <= CurRect.x + CurRect.height; ++i)
            {
                for (int j = CurRect.y - 1; j <= CurRect.y + CurRect.width; ++j)
                {
                    if (i <= c_x && (i * (b_y - a_y) >= ((b_x - a_x) * (j - b_y) + b_x * (b_y - a_y))) && (i * (b_y - c_y) <= ((b_x - c_x) * (j - b_y) + b_x * (b_y - c_y))))
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
                                if (nx <= c_x && (nx * (b_y - a_y) >= ((b_x - a_x) * (ny - b_y) + b_x * (b_y - a_y))) && (nx * (b_y - c_y) <= ((b_x - c_x) * (ny - b_y) + b_x * (b_y - c_y))))
                                {
                                    base_map[i][j] = EMapGeneratorData::Door;
                                    break;
                                }
                            }
                        }
                        else if (EMapGeneratorData::Door == base_map[i][j])
                        {
                            continue;
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
    case URectRoomMapGenerator::RoomType::Rhombus:
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

            if ((2 * point_x + 1) * (2 * point_y + 1) / 2 < min_room_size)
            {
                clean_room = true;
                break;
            }

            for (int i = CurRect.x - 1; i <= CurRect.x + CurRect.height; ++i)
            {
                for (int j = CurRect.y - 1; j <= CurRect.y + CurRect.width; ++j)
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
                        else if (EMapGeneratorData::Door == base_map[i][j])
                        {
                            continue;
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
    case URectRoomMapGenerator::RoomType::Circle:
    {
        //circle
        int mid_x = CurRect.x + (CurRect.height - 1) / 2;
        int mid_y = CurRect.y + (CurRect.width - 1) / 2;
        int radius = CurRect.height > CurRect.width ? CurRect.width / 2 : CurRect.height / 2;

        if (static_cast<int>(radius * radius * 3.141592f) < min_room_size)
        {
            clean_room = true;
            break;
        }
        for (int i = CurRect.x - 1; i <= CurRect.x + CurRect.height; ++i)
        {
            for (int j = CurRect.y - 1; j <= CurRect.y + CurRect.width; ++j)
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
                    else if (EMapGeneratorData::Door == base_map[i][j])
                    {
                        continue;
                    }
                    else
                    {
                        base_map[i][j] = EMapGeneratorData::Wall;
                    }
                }
            }
        }
        break;
    }
    default:
        clean_room = true;
        break;
    }

    if (true == clean_room)
    {
		for (int i = CurRect.x; i < CurRect.x + CurRect.height; ++i)
		{
			for (int j = CurRect.y; j < CurRect.y + CurRect.width; ++j)
			{
				if (EMapGeneratorData::Passage == base_map[i][j])
				{
					base_map[i][j] = EMapGeneratorData::Ground;
				}
			}
		}


		for (int i = CurRect.x - 1; i <= CurRect.x + CurRect.height; ++i)
		{
			if (EMapGeneratorData::Passage == base_map[i][CurRect.y - 1])
			{
				base_map[i][CurRect.y - 1] = EMapGeneratorData::Door;
			}
			if (EMapGeneratorData::Passage == base_map[i][CurRect.y + CurRect.width])
			{
				base_map[i][CurRect.y + CurRect.width] = EMapGeneratorData::Door;
			}
		}
		for (int j = CurRect.y - 1; j <= CurRect.y + CurRect.width; ++j)
		{
			if (EMapGeneratorData::Passage == base_map[CurRect.x - 1][j])
			{
				base_map[CurRect.x - 1][j] = EMapGeneratorData::Door;
			}
			if (EMapGeneratorData::Passage == base_map[CurRect.x + CurRect.height][j])
			{
				base_map[CurRect.x + CurRect.height][j] = EMapGeneratorData::Door;
			}
		}
    }
    return;
}
