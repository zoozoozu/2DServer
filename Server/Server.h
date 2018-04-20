#pragma once
#include "ServerPlayer.h"

class CServer {
	SOCKET			m_mysocket;
	WSABUF			send_wsabuf;
	char 			send_buffer[BUF_SIZE];
	WSABUF			recv_wsabuf;
	char			recv_buffer[BUF_SIZE];
	char			packet_buffer[BUF_SIZE];
	DWORD			in_packet_size = 0;
	int				saved_packet_size = 0;
	int				g_myid;

	HWND			m_hwnd;
	HINSTANCE		m_hist;

public:
	CServer();
	~CServer();

public:
	CServer_Player g_myClient;
	CServer_Player g_CloneClient[MAX_USER];

	// ½Ì±ÛÅæ »ç¿ëÀ» À§ÇÔ
	static CServer* getInstance()
	{
		static CServer inst;
		return &inst;
	}

	void Server_Init();
	void ProcessPacket(char *ptr);
	void ReadPacket();

	void setHwnd(HWND hwnd);
	void setInstance(HINSTANCE ins);
};