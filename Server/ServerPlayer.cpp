#include "stdafx.h"
#include "ServerPlayer.h"

CServer_Player::CServer_Player()
{
}

CServer_Player::~CServer_Player()
{
}

void CServer_Player::set_id(int id)
{
	this->id = id;
}

void CServer_Player::SetCoordX(int CoordX)
{
	this->m_szCoord.width = CoordX;
}

void CServer_Player::SetCoordY(int CoordY)
{
	this->m_szCoord.height = CoordY;
}

void CServer_Player::SetPosY(int CoordY)
{
	this->m_szCoord.height -= CoordY;
}

void CServer_Player::SetPosX(int CoordX)
{
	this->m_szCoord.width += CoordX;
}

