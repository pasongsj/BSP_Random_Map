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


    // spare�� �÷����� ������ �������� �ִ��� Ȯ���Ѵ�.
    while (true)
    {
        // �׽�Ʈ�� ���� �����Ѵ�
        CpyMap();


        // �� ����


		bool can_gen = true;
        if (CurMapShape == MapShape::none)
        {
            // �� ũ���� ���� ����
            float ratio = static_cast<float>(GetRoomSize({ 1, 1, lx - 2, ly - 2 })) / ((min_room_size + 1) * 2 * room_cnt);
            float _rate = ratio * spare + 0.1f < 1.0f ? 1.0f : ratio * spare + 0.1f;

			if (nullptr == RootNode)
			{
				RootNode = new Node();
				RootNode->nodeRect = { 1, 1, lx - 2, ly - 2 }; // ���� �׵θ��� ������ ũ���� ��Ʈ���
			}
			can_gen = DivideNode(RootNode, room_cnt, min_room_size, _rate, true);
        }
        else
        {
            
            int remain_room = room_cnt % ShapeList.size();
            for (Node* Roots : ShapeList)
            {
                // �� ũ���� ���� ����
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
       

        if (true == can_gen) // ������ ���� ����
        {
            break;
        }
        else
        {
            // ��� release
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
            if (spare >= 1.0f) // ������ ���� ����
            {
                printf("err");
                return false;
            }
        }
    }

    for (int i = 0; i < lx; ++i) // ������ ������ ������ ��� cpy
    {
        for (int j = 0; j < ly; ++j)
        {
            base_map[i][j] = try_map_gen[i][j];
        }
    }

    //��� �����ϱ�
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


         //�� ��� �����
    for (Node* _leaf : LeafNodeList)
    {
        CreateRoom(_leaf);
    }
    // ��� release
    LeafNodeList.clear();
    ReleaseNode(RootNode);
    for (Node* _node : ShapeList)
    {
        ReleaseNode(_node);
    }
    RootNode = nullptr;

    return true;

}

bool URectRoomMapGenerator::ConnectRoom(Node* main_node, Node* sub_node) // 1 �� 2�� ������, 2�� nullptr�� ���� ����
{
    // door ���� �ӽ� ����
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
    
    // x��(����)�� �������� Ȯ��
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
    if (n == 1) // ���̻� ���� ���� �ʿ䰡 ���� ��
    {
        if (_size > GetRoomSize(tree->nodeRect)) // �ּ� �� ũ�⸦ �������� �ʴ´ٸ�
        {
            return false;
        }
        LeafNodeList.push_back(tree);
        return true;
    }


    // ���� �� ������ �� ��

    // ���� ����� �� ����
    RectInt curRect = tree->nodeRect;

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
        curRect = RightRect;
        maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;;
        is_height = curRect.height > curRect.width ? true : false;

        // 5:5 ������ �����ٰ� ����
        split = maxLength / 2;

        // ���ʳ���� �� ����
        leftnodecnt = n / 2;
        // ������ ����� �� ����
        rightnodecnt = n - leftnodecnt;

        // �ִ� �� ũ�� * ���� ����
        max_room_size = static_cast<int>((_size + 1) * 2 * _rate);


        // 5:5�� �������� �� Left�� Right�ӽð�
        GetChildRect(curRect, split, is_height, LeftRect, RightRect);

        leftSize = GetRoomSize(LeftRect);
        rightSize = GetRoomSize(RightRect);
    }
    else if (rightSize == 0)
    {
        curRect = LeftRect;
        maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;;
        is_height = curRect.height > curRect.width ? true : false;

        // 5:5 ������ �����ٰ� ����
        split = maxLength / 2;

        // ���ʳ���� �� ����
        leftnodecnt = n / 2;
        // ������ ����� �� ����
        rightnodecnt = n - leftnodecnt;

        // �ִ� �� ũ�� * ���� ����
        max_room_size = static_cast<int>((_size + 1) * 2 * _rate);


        // 5:5�� �������� �� Left�� Right�ӽð�
        GetChildRect(curRect, split, is_height, LeftRect, RightRect);

        leftSize = GetRoomSize(LeftRect);
        rightSize = GetRoomSize(RightRect);
    }


    // Ȧ������ ���� �������ϴ� ��� 1 : 2 ũ�� ������ split�� ����ϱ� ����
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

    // split ��ġ�� �簢���� ��� ���
    if (split < 1 || split > maxLength - 1)
    {
        return false;
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
                    // Ȧ������ ���� �������ϴ� ��� 1 : 2 ũ�� ������ split�� ����ϱ� ����
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
        // ���� �ּ� ũ�⸦ ���߱� �����̴�.

        //���� ��忡 ���� ������ 
        tree->leftNode = new Node();
        tree->leftNode->nodeRect = LeftRect;

        //���� ��忡 ���� ������ 
        tree->rightNode = new Node();
        tree->rightNode->nodeRect = RightRect;

        //�� �� �� �ΰ��� ��带 ������ ���� �׸��� �Լ��̴�.        
        DrawLine(curRect, split, is_height, n);
    }

    tree->leftNode->parNode = tree; //�ڽĳ����� �θ��带 �������� ���� ����
    tree->rightNode->parNode = tree;

    // ���к����� �����
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;
    if (true == is_reverse && CurMapShape!= MapShape::none) // ����
    {
        return DivideNode(tree->rightNode, rightnodecnt, _size, nrate, true) && DivideNode(tree->leftNode, leftnodecnt, _size, nrate, false); //����, ������ �ڽ� ���鵵 �����ش�.
    }
    else // ����
    {
        return DivideNode(tree->leftNode, leftnodecnt, _size, nrate, true) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate, false); //����, ������ �ڽ� ���鵵 �����ش�.
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

//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
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

        // �簢��
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
        // ������
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
                    //| x - centerX | / a + | y - centerY | / b <= 1 ������ ����
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
