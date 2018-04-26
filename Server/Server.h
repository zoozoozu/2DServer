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
	CServer_Player g_Client[10];
	int client_id;		// 클라이언트 스스로 아이디를 저장하는 변수

	// 싱글톤 사용을 위함
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

	// Send Packet
	void Send_up(DWORD position);
	void Send_down(DWORD position);
	void Send_right(DWORD position);
	void Send_left(DWORD position);
};