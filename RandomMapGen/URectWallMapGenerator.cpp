#include "URectWallMapGenerator.h"


URectWallMapGenerator::URectWallMapGenerator() 
{
}

URectWallMapGenerator::~URectWallMapGenerator() 
{
}
bool URectWallMapGenerator::TryDiv()
{
    // �׽�Ʈ�� ���� �����Ѵ�

    CpyMap();
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
        can_gen = DivideNode(RootNode, room_cnt, min_room_size, _rate);
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
                can_gen = can_gen && DivideNode(Roots, room_cnt / static_cast<int>(ShapeList.size()) + 1, min_room_size, _rate);
                remain_room--;
            }
            else
            {
                can_gen = can_gen && DivideNode(Roots, room_cnt / static_cast<int>(ShapeList.size()), min_room_size, _rate);
            }
        }
    }
    ReleaseNode(RootNode);
    RootNode = nullptr;
    return can_gen;
}

bool URectWallMapGenerator::CreateMap()
{
	URectMapGenerator::CreateMap();

    // ���� �����ϱ�
    switch (CurMapShape)
    {
    case MapShape::none:
        break;
    case MapShape::giyeok:
    case MapShape::digeut:
    case MapShape::mieum:
    {
        for (int i = 0; i < ShapeList.size() - 1; ++i)
        {
            MakeDoor(ShapeList[i], ShapeList[i + 1]);
        }
        break;
    }
    case MapShape::cross:
    {

        for (int i = 1; i < ShapeList.size(); ++i)
        {
            MakeDoor(ShapeList[0], ShapeList[i]);
        }
        break;
    }
    default:
        break;
    }

    // spare�� �÷����� ������ �������� �ִ��� Ȯ���Ѵ�.
    // ������ Ʈ����
    while (true)
    {
        // �� ����
        m_rate = 0.2f;
        bool can_gen = true;
		for (int k = 0; k < 5; ++k)
		{
			m_rate = 0.2f;
			can_gen = TryDiv();

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

			if (true == can_gen)
			{
				break;
			}
		}
        spare += 0.01f;
        if (spare >= 1.0f) // ������ ���� ����
        {
            // ��� release
            for (Node* _node : ShapeList)
            {
                ReleaseNode(_node);
            }
            return false;
        }
        if (true == can_gen) // ������ ���� ����
        {
			break;
        }
    }

    for (int i = 0; i < lx; ++i) // ������ ������ ������ ��� cpy
    {
        for (int j = 0; j < ly; ++j)
        {
            base_map[i][j] = try_map_gen[i][j];
        }
    }


    // ��� release
    for (Node* _node : ShapeList)
    {
        ReleaseNode(_node);
    }

    return true;

}


bool URectWallMapGenerator::CreateMap(const std::vector<std::vector<EMapGeneratorData>>& _map, int _roomcnt, int _min_room_size, int _doorsize, MapShape _Shape )
{
    isDone = false;
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;
    CurMapShape = _Shape;

    if (true == URectWallMapGenerator::CreateMap())
    {
        //for (int i = 0; i < lx; ++i)
        //{
        //    for (int j = 0; j < ly; ++j)
        //    {
        //        _map[i][j] = base_map[i][j];
        //    }
        //}
        return isDone = true, true;

    }
    isDone = true;
    return false;
}


// ���� Node�� n���� ������ʹٴ� �ǹ�
bool URectWallMapGenerator::DivideNode(Node* tree, int n, int _size, float _rate)
{
    if (n == 0)
    {
        FillTryMapRect(tree->nodeRect, EMapGeneratorData::Wall);
        return true;
    }
    if (n == 1) // ���̻� ���� ���� �ʿ䰡 ���� ��
    {
        if (_size > GetRoomSize(tree->nodeRect)) // �ּ� �� ũ�⸦ �������� �ʴ´ٸ�
        {
            return false;
        }
        //LeafNodeList.push_back(tree);
        return true;
    }

    // ���� �� ������ �� ��

    // ���� ����� �� ����
    const RectInt curRect = tree->nodeRect;

    // ���η� ������ ���η� ������
    int maxLength = curRect.height > curRect.width ? curRect.height : curRect.width;
    if (maxLength <= door_size + 1)
    {
        return false;
    }

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
        tree->rightNode = new Node();
        tree->rightNode->nodeRect = RightRect;
        tree->rightNode->parNode = tree;
        return DivideNode(tree->rightNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
    }
    else if (rightSize == 0)
    {
        tree->rightNode = new Node();
        tree->rightNode->nodeRect = LeftRect;
        tree->rightNode->parNode = tree;
        return DivideNode(tree->rightNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
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
                if (lim> split - 1)
                {
                    return false;
                }
                split = GameEngineRandom::MainRandom.RandomInt(lim, split-1);
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
        if (false == DrawLine(curRect, split, is_height, n))
        {
            return false;
        }
    }

    tree->leftNode->parNode = tree; //�ڽĳ����� �θ��带 �������� ���� ����
    tree->rightNode->parNode = tree;

    // ���к����� �����
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;

    return DivideNode(tree->leftNode, leftnodecnt, _size, nrate) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate); //����, ������ �ڽ� ���鵵 �����ش�.

}

bool URectWallMapGenerator::MakeDoor(Node* _fnode, Node* _snode)
{
    if (_fnode->nodeRect.x == _snode->nodeRect.x)
    {
        RectInt fRect = _fnode->nodeRect.y > _snode->nodeRect.y ? _snode->nodeRect : _fnode->nodeRect;
        RectInt sRect = _fnode->nodeRect.y < _snode->nodeRect.y ? _snode->nodeRect : _fnode->nodeRect;

        int mid = fRect.x + fRect.height / 2;
        int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - fRect.height / 4, mid + fRect.height / 4);

        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
        {
			if (EMapGeneratorData::Wall == base_map[i][fRect.y + fRect.width ])
            {
				base_map[i][fRect.y + fRect.width] = EMapGeneratorData::Door;
            }
        }
        return true;
    }
    else if (_fnode->nodeRect.y == _snode->nodeRect.y)
    {

        RectInt fRect = _fnode->nodeRect.x > _snode->nodeRect.x ? _snode->nodeRect : _fnode->nodeRect;
        RectInt sRect = _fnode->nodeRect.x < _snode->nodeRect.x ? _snode->nodeRect : _fnode->nodeRect;

        int mid = fRect.y + fRect.width / 2;
        int doorPos = GameEngineRandom::MainRandom.RandomInt(mid - fRect.width / 4, mid + fRect.width / 4);

        for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
        {
			if (EMapGeneratorData::Wall == base_map[fRect.x + fRect.height][i])
            {
				base_map[fRect.x + fRect.height][i] = EMapGeneratorData::Door;
            }
        }
        return true;

    }
    else
    {
        return false;
    }
}


//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
bool URectWallMapGenerator::DrawLine(const RectInt& _cur, int splite, bool is_height, int n)
{
    if (is_height)
    {
        int mid = _cur.y + _cur.width / 2;
        int diff = static_cast<int>(_cur.width * 0.35f / n);
        std::vector<int> candoorvec;
        for (int i = _cur.y; i < _cur.y + _cur.width; ++i)
        {
            if (EMapGeneratorData::Ground == try_map_gen[_cur.x + splite][i])
            {
                try_map_gen[_cur.x + splite][i] = EMapGeneratorData::Wall;
                candoorvec.push_back(i);
            }
        }
        if (static_cast<int>(candoorvec.size()) - door_size < 0)
        {
            return false;
        }
        int doorindex = GameEngineRandom::MainRandom.RandomInt(0, static_cast<int>(candoorvec.size()) - door_size);
        for (int i = 0; i < door_size; ++i)
        {
			try_map_gen[_cur.x + splite][candoorvec[doorindex + i]] = EMapGeneratorData::Door;
        }
    }
    else
    {
        int mid = _cur.x + _cur.height / 2;
        int diff = static_cast<int>(_cur.height * 0.35f / n);
        std::vector<int> candoorvec;
        for (int i = _cur.x; i < _cur.x + _cur.height; ++i)
        {
            if (EMapGeneratorData::Ground == try_map_gen[i][_cur.y + splite])
            {
                try_map_gen[i][_cur.y + splite] = EMapGeneratorData::Wall;
                candoorvec.push_back(i);

            }
        }

        if (static_cast<int>(candoorvec.size()) - door_size < 0)
        {
            return false;
        }
        int doorindex = GameEngineRandom::MainRandom.RandomInt(0, static_cast<int>(candoorvec.size()) - door_size);
        for (int i = 0; i < door_size; ++i)
        {
			try_map_gen[candoorvec[doorindex + i]][_cur.y + splite] = EMapGeneratorData::Door;
        }

    }
    return true;
}