#include "URectWallMapGenerator.h"


URectWallMapGenerator::URectWallMapGenerator() 
{
}

URectWallMapGenerator::~URectWallMapGenerator() 
{
}


bool URectWallMapGenerator::CreateMap()
{
	URectMapGenerator::CreateMap();


    float ratio = static_cast<float>(GetRoomSize({ 1, 1, lx - 1, ly - 1 })) / ((min_room_size + 1) * 2 * room_cnt);

    // spare�� �÷����� ������ �������� �ִ��� Ȯ���Ѵ�.
    while (true)
    {
        // �׽�Ʈ�� ���� �����Ѵ�
        CpyMap();

        // �� ũ���� ���� ����
        float _rate = ratio * spare + 0.1f < 1.0f ? 1.0f : ratio * spare + 0.1f;

        // �� ����
        if (nullptr == RootNode)
        {
            RootNode = std::make_shared<Node>();
            RootNode->nodeRect = { 1, 1, lx - 1, ly - 1 }; // ���� �׵θ��� ������ ũ���� ��Ʈ���
        }
        bool can_gen = DivideNode(RootNode, room_cnt, min_room_size, _rate);

        if (true == can_gen) // ������ ���� ����
        {
            break;
        }
        else
        {
            // ��� release
            ReleaseNode(RootNode);
            RootNode = nullptr;
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


    // -- test --


    // ��� release
    ReleaseNode(RootNode);
    RootNode = nullptr;

    return true;

}


bool URectWallMapGenerator::CreateMap(std::vector<std::vector<int>> _map, int _roomcnt, int _min_room_size, int _doorsize)
{
    base_map = _map;

    lx = static_cast<int>(_map.size());
    ly = static_cast<int>(_map[0].size());

    room_cnt = _roomcnt;
    min_room_size = _min_room_size;
    door_size = _doorsize;

    spare = 0.8f;
    return URectWallMapGenerator::CreateMap();
}


// ���� Node�� n���� ������ʹٴ� �ǹ�
bool URectWallMapGenerator::DivideNode(std::shared_ptr<Node> tree, int n, int _size, float _rate)
{
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
        tree->rightNode = std::make_shared<Node>();
        tree->rightNode->nodeRect = RightRect;
        tree->rightNode->parNode = tree;
        return DivideNode(tree->rightNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
    }
    else if (rightSize == 0)
    {
        tree->leftNode = std::make_shared<Node>();
        tree->leftNode->nodeRect = LeftRect;
        tree->leftNode->parNode = tree;
        return DivideNode(tree->leftNode, n, _size, _rate);//����, ������ �ڽ� ���鵵 �����ش�.
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
        tree->leftNode = std::make_shared<Node>();
        tree->leftNode->nodeRect = LeftRect;

        //���� ��忡 ���� ������ 
        tree->rightNode = std::make_shared<Node>();
        tree->rightNode->nodeRect = RightRect;

        //�� �� �� �ΰ��� ��带 ������ ���� �׸��� �Լ��̴�.        
        DrawLine(curRect, split, is_height, n);
    }

    tree->leftNode->parNode = tree; //�ڽĳ����� �θ��带 �������� ���� ����
    tree->rightNode->parNode = tree;

    // ���к����� �����
    float nrate = _rate * spare > 1.0 ? _rate * spare : 1.0f;

    return DivideNode(tree->leftNode, leftnodecnt, _size, nrate) && DivideNode(tree->rightNode, rightnodecnt, _size, nrate); //����, ������ �ڽ� ���鵵 �����ش�.

}

//�ڽ� ��带 ����� ���м��� �׸��� �Լ� _cur �簢���� ���� splite���м��̴�
void URectWallMapGenerator::DrawLine(const RectInt& _cur, int splite, bool is_height, int n)
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

		for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
		{
			if (try_map_gen[_cur.x + splite][i] != -1)
			{
				try_map_gen[_cur.x + splite][i] = 2;
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

		for (int i = doorPos - door_size / 2; i < doorPos + door_size / 2 + (door_size & 1); ++i)
		{
			if (try_map_gen[i][_cur.y + splite] != -1)
			{
				try_map_gen[i][_cur.y + splite] = 2;
			}
		}

    }
}