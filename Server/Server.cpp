#include "stdafx.h"
#include "Server.h"

CServer::CServer()
{
}

CServer::~CServer()
{
}

void CServer::Server_Init()
{
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(m_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(m_mysocket, m_hwnd, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;
}

void CServer::ProcessPacket(char * ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		int id = my_packet->id;
		if (first_time) {
			first_time = false;
			g_myid = id;
		}
		if (id == g_myid) {
			g_myClient.SetCoordX(my_packet->x);
			g_myClient.SetCoordY(my_packet->y);
			//player.attr |= BOB_ATTR_VISIBLE;
		}
		else if (id < NPC_START) {
			g_CloneClient[id].SetCoordX(my_packet->x);
			g_CloneClient[id].SetCoordY(my_packet->y);
			//g_CloneClient[id].attr |= BOB_ATTR_VISIBLE;
		}
	/*	else {
			npc[id - NPC_START].x = my_packet->x;
			npc[id - NPC_START].y = my_packet->y;
			npc[id - NPC_START].attr |= BOB_ATTR_VISIBLE;
		}*/
		break;
	}
	case SC_POS:
	{
		sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			g_myClient.SetCoordX(my_packet->x);
			g_myClient.SetCoordY(my_packet->y);
		}
		else if (other_id < NPC_START) {
			g_CloneClient[other_id].SetCoordX(my_packet->x);
			g_CloneClient[other_id].SetCoordY(my_packet->y);
		}
		//else {
		//	npc[other_id - NPC_START].x = my_packet->x;
		//	npc[other_id - NPC_START].y = my_packet->y;
		//}
		break;
	}

	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player *my_packet = reinterpret_cast<sc_packet_remove_player *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			// 아주 멀리 날려버리기.
			g_myClient.SetCoordX(1000.f);
			g_myClient.SetCoordY(1000.f);
			//player.attr &= ~BOB_ATTR_VISIBLE;
		}
		else if (other_id < NPC_START) {
			g_CloneClient[other_id].SetCoordX(1000.f);
			g_CloneClient[other_id].SetCoordY(1000.f);
			//skelaton[other_id].attr &= ~BOB_ATTR_VISIBLE;
		}
		/*else {
			npc[other_id - NPC_START].attr &= ~BOB_ATTR_VISIBLE;
		}*/
		break;
	}
	case SC_CHAT:
	{
	/*	sc_packet_chat *my_packet = reinterpret_cast<sc_packet_chat *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			wcsncpy_s(player.message, my_packet->message, 256);
			player.message_time = GetTickCount();
		}
		else if (other_id < NPC_START) {
			wcsncpy_s(skelaton[other_id].message, my_packet->message, 256);
			skelaton[other_id].message_time = GetTickCount();
		}
		else {
			wcsncpy_s(npc[other_id - NPC_START].message, my_packet->message, 256);
			npc[other_id - NPC_START].message_time = GetTickCount();
		}
		break;*/

	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void CServer::ReadPacket()
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(m_mysocket, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void CServer::setHwnd(HWND hwnd)
{
	m_hwnd = hwnd;
}

void CServer::setInstance(HINSTANCE ins)
{
	m_hist = ins;
}
