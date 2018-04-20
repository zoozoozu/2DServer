#pragma once

class CServer_Player
{
	D2D_SIZE_U		m_szCoord;
	int id;
	bool is_connected{ false };

public:
	CServer_Player();
	~CServer_Player();
	static CServer_Player* getInstance()
	{
		static CServer_Player inst;
		return &inst;
	}

	void SetCoordX(int CoordX);
	void SetCoordY(int CoordY);
};