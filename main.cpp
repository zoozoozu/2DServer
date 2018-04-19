// main.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include "Framework/IndRes/IndRes.h"
#include "Framework/Timer/Timer.h"
#include "Framework/Warp2DFramework.h"

#include "main.h"

#define MAX_LOADSTRING 100



namespace 
{
	HINSTANCE hInst;                                // 현재 인스턴스입니다.
	WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
	WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

	shared_ptr<CIndRes> ind = make_shared<CIndRes>();
	shared_ptr<CTimer> timer = make_shared<CTimer>();
	CWarp2DFramework framework;

	int g_iMarginWidth = 0;
	int g_iMarginHeight = 0;
}

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

int GetMarginWidth() { return g_iMarginWidth; }
int GetMarginHeight() { return g_iMarginHeight; }
HINSTANCE GetHInstance() { return hInst; }

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
	if (!ind->Initialize()) return FALSE;
	
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAIN));

    MSG msg;

    // 기본 메시지 루프입니다.

    while (true)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) continue;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!timer->Update()) continue;
		framework.FrameAdvance();

    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = CWarp2DFramework::WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//	윈도우 스타일
	DWORD dwStyle = 0
		| WS_OVERLAPPED 	// 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
		| WS_CAPTION 		// 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
		| WS_SYSMENU		// 시스템 메뉴를 가진 윈도우를 만든다.
		| WS_MINIMIZEBOX	// 최소화 버튼을 만든다.
		| WS_BORDER			// 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
		| WS_THICKFRAME		// 크기 조정이 가능한 두꺼운 경계선을 가진다. WS_BORDER와 같이 사용할 수 없다.
		;					// 추가로 필요한 윈도우 스타일은 http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm 참고.

	//	인스턴스 핸들을 전역 변수에 저장
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	//	데스크톱 윈도우 사이즈
	RECT getWinSize;
	::GetWindowRect(::GetDesktopWindow(), &getWinSize);

	//	클라이언트 사이즈
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	//	윈도우 사이즈에 실제로 추가되는(캡션, 외곽선 등) 크기를 보정.
	::AdjustWindowRect(&rc, dwStyle, FALSE);

	g_iMarginWidth = rc.right - rc.left - CLIENT_WIDTH;
	g_iMarginHeight = rc.bottom - rc.top - CLIENT_HEIGHT;

	//	클라이언트 절대좌표(left, top)
	//	데스크톱의 중앙에 클라이언트가 위치하도록 설정
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

	//	윈도우 생성
	HWND hWnd = CreateWindow(
		  szWindowClass			//	윈도우 클래스 명
		, szTitle				//	캡션 표시 문자열
		, dwStyle				//	윈도우 스타일
		, ptClientWorld.x		//	부모 윈도우 기반 윈도우 시작좌표 : x
		, ptClientWorld.y		//	부모 윈도우 기반 윈도우 시작좌표 : y
		, rc.right - rc.left	//	윈도우 사이즈 : width
		, rc.bottom - rc.top	//	윈도우 사이즈 : height
		, NULL					//	부모 윈도우.
		, NULL					//	메뉴 핸들
		, hInstance				//	인스턴스 핸들
		, NULL					//	추가 파라메터 : NULL
		);

	//	생성 실패시 프로그램 종료
	//	확인 : WndProc의 default msg handler가 DefWindowProc 함수를 반환하는가?
	if (!hWnd)
	{
		LPVOID lpMsgBuf;
		FormatMessage(	  FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
						, NULL
						, GetLastError()
						, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
						, reinterpret_cast<LPTSTR>(&lpMsgBuf)
						, 0
						, NULL
		);
		MessageBox(NULL, reinterpret_cast<LPCTSTR>(lpMsgBuf), L"Create Window Fail", MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return FALSE;
	}

	timer->SetUpdateCaptionHwnd(hWnd);
	framework.OnCreate(hWnd, hInst, ind, timer);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
