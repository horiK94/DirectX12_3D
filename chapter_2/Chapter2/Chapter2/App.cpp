#include "App.h"
namespace
{
	const auto ClassName = TEXT("SampleWindowClass");
}
/// <summary>
/// コンストラクタ
/// </summary>
App::App(uint32_t width, uint32_t height)
	: m_hInst(nullptr)
	, m_hWnd(nullptr)
	, m_Width(width)
	, m_Height(height)
{
}

App::~App()
{
}

void App::Run()
{
	if (InitApp())
	{
		MainLoop();
	}

	//アプリケーション終了処理
	TermApp();
}

bool App::InitApp()
{
	return InitWnd();
}

bool App::InitWnd()
{
	//インスタンスハンドルの取得
	auto hInst = GetModuleHandle(nullptr);
	if (hInst == nullptr)
	{
		return false;
	}

	//ウィンドウの設定
	WNDCLASSEX wc = {};
	//構造体のサイズ
	wc.cbSize = sizeof(WNDCLASSEX);
	//ウィンドウのスタイルを決める
	//CS_HREDRAW: 水平方向のサイズ変更時にウィンドウ再描画
	//CS_VREDRAW: 垂直方向のサイズ変更時にウィンドウ再描画
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//ウィンドウプロシージャルの指定
	wc.lpfnWndProc = WndProc;
	//インスタンスハンドルの設定. なくてもよいっぽい
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(hInst, IDC_ARROW);
	//ウィンドウの背景色
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	//ウィンドウクラスを識別する名前
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION);

	//ウィンドウの登録
	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	//インスタンスハンドルの設定
	m_hInst = hInst;

	//ウィンドウサイズの設定
	RECT rt = {};
	rt.right = static_cast<LONG>(m_Width);
	rt.bottom = static_cast<LONG>(m_Height);

	//作成するウィンドウのスタイル. 最小限で設定するほうが、より効率的な処理を行ってくれる
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	//ウィンドウの生成
	m_hWnd = CreateWindowEx(
		0,
		//ウィンドウ識別名
		ClassName,
		TEXT("Test"),
		style,
		//ウィンドウの横方向の初期位置. 今回はデフォルト値指定
		CW_USEDEFAULT,
		//ウィンドウの縦方向の初期位置. 今回はデフォルト値指定
		CW_USEDEFAULT,
		//ウィンドウ幅
		rt.right - rt.left,
		rt.bottom - rt.top,
		//親ウィンドウの指定
		nullptr,
		//メニュー、子ウィンドウの指定
		nullptr,
		//ウィンドウのインスタンハンドル
		m_hInst,
		//ウィンドウに渡すパラメータ
		nullptr);

	if (m_hWnd == nullptr)
	{
		//生成の失敗
		return false;
	}

	//ウィンドウの表示. 第2引数はウィンドウの表示設定
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	//ウィンドウをフォーカスさせる
	SetFocus(m_hWnd);

	return true;
}

//ウィンドウプロシージャ
//WindowsOSから来たメッセージキューが通知される関数
LRESULT CALLBACK App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:		//ウィンドウ破棄
	{
		PostQuitMessage(0);
	}
	break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}


void App::MainLoop()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		//アプリケーション宛にメッセージが送られているか. 送られていればmsgに代入される
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
		{
			TranslateMessage(&msg);		//キーボードの入力を実際の文字に変換
			DispatchMessage(&msg);		//ウィンドウプロシージャルに送る
		}
	}
}

void App::TermApp()
{
	//ウィンドウの終了処理
	TermWnd();
}

void App::TermWnd()
{
	//ウィンドウの登録を解除
	if (m_hInst != nullptr)
	{
		UnregisterClass(ClassName, m_hInst);
	}

	m_hInst = nullptr;
	m_hWnd = nullptr;
}