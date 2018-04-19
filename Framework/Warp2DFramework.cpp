#include "stdafx.h"

#include "IndRes/IndRes.h"
#include "Timer/Timer.h"
#include "Scene/Scene.h"
#include "Scene/Test/TestScene.h"

#include "Warp2DFramework.h"

CWarp2DFramework::CWarp2DFramework() 
{
}

CWarp2DFramework::~CWarp2DFramework()
{
}

void CWarp2DFramework::OnCreate(HWND hWnd, HINSTANCE hInst, shared_ptr<CIndRes> indres, shared_ptr<CTimer> timer)
{
	RegisterIndRes(indres);
	RegisterTimer(timer);

	m_hWnd = hWnd;
	m_hInst = hInst;
	::GetClientRect(hWnd, &m_rcClient);

	// Ŭ������ ������ ���ν��� ����
	::SetUserDataPtr(m_hWnd, this);

	m_pIndRes->CreateHwndRenderTarget(hWnd, &m_pd2dRenderTarget);

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(g_mysocket, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	BuildScene<CTestScene>(L"Test"s);
}

void CWarp2DFramework::BuildScene(wstring Tag, const unique_ptr<CScene>& scene)
{
	scene->OnCreate(move(Tag), this);
}

void CWarp2DFramework::FrameAdvance()
{
	Update(m_pTimer->GetTimeElapsed());
	Draw();
}

void CWarp2DFramework::Draw()
{
	m_pd2dRenderTarget->BeginDraw();
	m_pd2dRenderTarget->Clear(ColorF{ ColorF::LightGray });

	if (m_pCurrentScene) m_pCurrentScene->Draw(m_pd2dRenderTarget.Get());

	m_pd2dRenderTarget->EndDraw();

}

void CWarp2DFramework::Update(float fTimeElapsed)
{
	if (m_pCurrentScene) m_pCurrentScene->Update(fTimeElapsed);
}



bool CWarp2DFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:	break;
	case WM_MBUTTONDOWN:	break;
	case WM_RBUTTONDOWN:	break;
	case WM_MOUSEMOVE:		break;
	case WM_LBUTTONUP:		break;
	case WM_MBUTTONUP:		break;
	case WM_RBUTTONUP:		break;
	case WM_MOUSEWHEEL:		break;
	default:
		return false;
	}

	return true;
}

bool CWarp2DFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_DESTROY, 0, 0);
			break;

		default:
			return false;
		}
		break;
	default:
		return false;
	}
	return true;
}

LRESULT CWarp2DFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	// ��ó�� ���� : ���� Scene���� ó���� ������ Ȯ��
	switch (nMessageID)
	{
	case WM_SIZE:
		{
			auto nWndClientWidth = static_cast<LONG>(LOWORD(lParam));
			auto nWndClientHeight = static_cast<LONG>(HIWORD(lParam));

			if (nWndClientWidth < CLIENT_MINIMUM_WIDTH - GetMarginWidth())
				nWndClientWidth = CLIENT_MINIMUM_WIDTH - GetMarginWidth();
				
			if (nWndClientHeight < CLIENT_MINIMUM_HEIGHT - GetMarginHeight())
				nWndClientHeight = CLIENT_MINIMUM_HEIGHT - GetMarginHeight();

			m_rcClient = RectL(0, 0, nWndClientWidth, nWndClientHeight);

			m_pd2dRenderTarget->Resize(SizeU(nWndClientWidth, nWndClientHeight));
			
			Draw();

			return DefWindowProc(hWnd, nMessageID, wParam, lParam);
		}

	default:
		if (m_pCurrentScene)	if (!m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam)) break;
								else return 0;
	}

	// ��ó�� ���� : Scene���� ó������ �ʰ� ���� �κ��� ó��
	switch (nMessageID)
	{
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = CLIENT_MINIMUM_WIDTH;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = CLIENT_MINIMUM_HEIGHT;

		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, nMessageID, wParam, lParam);
}

void CWarp2DFramework::ChangeScene(wstring Tag, bool bDestroyPostScene)
{
	auto pChangeScene = FindScene(Tag);
	if (!pChangeScene) return _DEBUG_ERROR("�������� ���� Scene�� �����Ϸ� �߽��ϴ�!");

	if (m_pCurrentScene && bDestroyPostScene)
	{
		auto& DestroyTag = m_pCurrentScene->Tag();

		m_pCurrentScene = nullptr;

		m_lstScenes.remove_if(
			[&] (const unique_ptr<CScene>& s)
		{ return s->FindByTag(DestroyTag); }
		);
	}

	m_pCurrentScene = pChangeScene;
}

CScene * CWarp2DFramework::FindScene(std::wstring Tag)
{
	auto ChangeScene = find_if(begin(m_lstScenes), end(m_lstScenes),
		[&] (const unique_ptr<CScene>& s)
		{ return s->FindByTag(Tag); }
	);
	return ChangeScene == end(m_lstScenes) ? nullptr : ChangeScene->get();
}



LRESULT CALLBACK CWarp2DFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	auto self = ::GetUserDataPtr<CWarp2DFramework*>(hWnd);
	if (!self) return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	static auto DestroyWindow = [&] ()
	{
		::SetUserDataPtr(hWnd, nullptr);
		::PostQuitMessage(0);
	};
	
	switch (nMessageID)
	{
	case WM_DESTROY:
		DestroyWindow();
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}
