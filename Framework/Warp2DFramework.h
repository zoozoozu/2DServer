#pragma once

class CIndRes;
class CTimer;

class CScene;



class CWarp2DFramework
{

public:
	CWarp2DFramework();
	~CWarp2DFramework();

	void RegisterIndRes(shared_ptr<CIndRes> indres) { m_pIndRes = indres; }
	void RegisterTimer(shared_ptr<CTimer> timer) { m_pTimer = timer; }

	// Framework ���� ��
	void OnCreate(HWND hWnd, HINSTANCE hInst, shared_ptr<CIndRes> indres, shared_ptr<CTimer> timer);


	// Scene ���� ��
	template<typename Scene>
	Scene* BuildScene(wstring Tag, bool bChangeThis = true)
	{
		static_assert(is_base_of_v<CScene, Scene>, "Scene is must be based on CScene!");

		unique_ptr<CScene> scene{ make_unique<Scene>() };
		BuildScene(Tag, scene);
		m_lstScenes.push_back(move(scene));
		if (bChangeThis) ChangeScene(Tag);

		return static_cast<Scene*>(FindScene(Tag));
	}

private:
	void BuildScene(wstring Tag, const unique_ptr<CScene>& scene);


public:

	void FrameAdvance();

	void Draw();

	void Update(float fTimeElapsed);



	// �� ó���� ���� ��� true ��ȯ
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �� ó���� ���� ��� true ��ȯ
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �� ó���� ���� ��� true ��ȯ
	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// Framework ���� ȣ���ϴ� ������ ���ν����Դϴ�.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void ChangeScene(std::wstring Tag, bool bDestroyPostScene = false);
	CScene* FindScene(std::wstring Tag);

	// Getter
public:
	HWND GethWnd()									const { return m_hWnd; }
	D2D_RECT_L GetClientSize()						const { return m_rcClient; }
	shared_ptr<CIndRes> GetIndRes()					const { return m_pIndRes; }
	ComPtr<ID2D1HwndRenderTarget> GetRenderTarget()	const { return m_pd2dRenderTarget; }

	// Framework�� �ʿ��� �͵�.
private:
	
	HWND							m_hWnd				{ nullptr }	;
	HINSTANCE						m_hInst				{ nullptr }	;

	shared_ptr<CIndRes>				m_pIndRes			{ nullptr }	;
	shared_ptr<CTimer>				m_pTimer			{ nullptr }	;

	ComPtr<ID2D1HwndRenderTarget>	m_pd2dRenderTarget	{ nullptr }	;

	D2D_RECT_L						m_rcClient						;

	list<unique_ptr<CScene>>		m_lstScenes						;
	CScene*							m_pCurrentScene		{ nullptr }	;

};
