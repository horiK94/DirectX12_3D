#include "App.h"

namespace
{
	const auto ClassName = TEXT("SampleWindowClass");		//ウィンドウクラス名
}

App::App(uint32_t width, uint32_t height) :
	m_Width(width), m_Height(height), m_hInst(nullptr), m_hWnd(nullptr)
{
}

App::~App()
{
}

void App::Run()
{
	if (InitApp()) {
		MainLoop();
	}

	TermApp();
}

bool App::InitApp()
{
	//ウィンドウ初期化
	if (!InitWnd())
	{
		return false;
	}

	return true;
}

//termはterminator(終了)の略だと思われる
void App::TermApp()
{
	//ウィンドウの終了処理
	TermWnd();
}

//ウィンドウ初期化
bool App::InitWnd()
{
	//インスタンハンドル作成
	auto hInst = GetModuleHandle(nullptr);
	{
		if (hInst == nullptr)
		{
			return false;
		}
	}

	//ウィンドウ生成
	{
		//ウィンドウの設定
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);		//構造体のサイズ
		wc.style = CS_HREDRAW | CS_VREDRAW;		//ウィンドウスタイル
		//CS_HREDRAW: 	横サイズが変わったとき，ウインドウ全体を再描画
		//CS_VREDRAW: 	縦サイズが変わったとき，ウインドウ全体を再描画
		wc.lpfnWndProc = WndProc;		//★ ウィンドウのメッセージを処理するコールバック関数
		wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);	//アイコンハンドル
		wc.hCursor = LoadCursor(hInst, IDC_ARROW);		//カーソルハンドル
		wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);		//ウィンドウ背景
		wc.lpszMenuName = nullptr;		//メニュー名
		wc.lpszClassName = ClassName;		//ウィンドウクラスにつける名前
		wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION); //16*16の小さいアイコン

		//ウインドウクラス登録
		if (!RegisterClassEx(&wc))
		{
			return false;
		}

		m_hInst = hInst;

		//ウィンドウのサイズ設定
		RECT rc = { };
		rc.right = static_cast<LONG>(m_Width);
		rc.bottom = static_cast<LONG>(m_Height);

		//ウィンドウスタイル設定
		auto style = WS_OVERLAPPED		//	タイトルバー，境界線つきオーバーラップウインドウ
			| WS_CAPTION		//	タイトルバー(WS_DLGFRAMEも一緒についてくる)
			| WS_SYSMENU;		// 	タイトルバーにシステムメニュー(閉じるボタンとアイコン)つける
		//LPREC = RECT * （Win32)
		// RECT rc; ⇒ LPRECT lprc = &rc;
		AdjustWindowRect(&rc, style, FALSE);		//styleと第3引数からrcに計算された値が入る -> CreateWindow()で使用

		//ウィンドウ生成
		m_hWnd = CreateWindowEx(		//CreateWindowの拡張関数
			0,		//拡張スタイル
			ClassName,		//ウィンドウクラス名を指定
			TEXT("Sample"),		// ウィンドウ名を指定。タイトルバーなどに表示される
			style,		//ウィンドウのスタイル
			CW_USEDEFAULT,		//ウィンドウの初期位置のx座標
			CW_USEDEFAULT,		//ウィンドウの初期位置のy座標
			rc.right - rc.left,		//ウィンドウの横幅
			rc.bottom - rc.top,		//ウィンドウの縦幅
			nullptr,		//作成されるウィンドウの親ウィンドウ or オーナーを指定
			nullptr,		//メニュー or 子ウィンドウIDを指定
			m_hInst,		//ウィンドウに関連づいたインスタンスハンドルを指定
			nullptr		//アプリがCreateWindowEx または CreateWindow 関数を呼び出してウィンドウの作成を要求したときに、ウィンドウプロシージャの WM_CREATE メッセージの lParam パラメータとして渡される構造体ポインタ
		);

		if (m_hWnd == nullptr)
		{
			return false;
		}

		//ウィンドウの表示状態を設定
		ShowWindow(m_hWnd, SW_SHOWNORMAL);		//SW_SHOWNORMAL: ウィンドウをアクティブにして、表示. 最大化 or 最小化のときは元に戻す

		//ウィンドウの再描画要求(=WM_PAINT メッセージを送る)
		UpdateWindow(m_hWnd);

		//指定したウィンドウをフォーカス状態にする
		SetFocus(m_hWnd);
	}

	return true;
}

void App::TermWnd()
{
	if (m_hInst != nullptr)
	{
		//ウィンドウクラスの登録解除
		UnregisterClass(ClassName, m_hInst);
	}

	m_hInst = nullptr;
	m_hWnd = nullptr;
}

void App::MainLoop()
{
	MSG msg = {};

	while(WM_QUIT != msg.message)
	{
		//スレッドのメッセージキューにメッセージがあるかどうかをチェック
		if (PeekMessage(&msg,		//メッセージ情報を格納するポインタを指定
			nullptr,		//メッセージを取得するウィンドウのハンドルを指定する。全ての場合は NULL
			0,		//メッセージの最小値を指定し、フィルタリング。しない場合は0
			0,		//メッセージの最大値を指定し、フィルタリング。しない場合は0
			PM_REMOVE))		//メッセージの処理方法. PM_REMOVE ならばメッセージをキューから削除
		{
			//メッセージ取得時はtrue
			TranslateMessage(&msg);		//キー入力を実際の文字に変換する
			DispatchMessage(&msg);		//ウィンドウプロシージャにメッセージを渡す
		}
	}
}

LRESULT App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:		//ウィンドウ破棄メッセージ
	{
		//WM_QUITメッセージキューを送る
		PostQuitMessage(0);
	}
		break;
	default:
		break;
	}
	
	//ウィンドウメッセージの規定動作を行う
	return DefWindowProc(hWnd, msg, wp, lp);
}
