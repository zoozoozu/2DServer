#pragma once

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

public:
	CServer();
	~CServer();
};