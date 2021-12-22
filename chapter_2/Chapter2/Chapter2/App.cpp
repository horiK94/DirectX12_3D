#include "App.h"

namespace
{
	const auto ClassName = TEXT("SampleWindowClass");

	template<typename T>
	void SafeRelease(T*& ptr)
	{
		if (ptr != nullptr)
		{
			ptr->Release();
			//適当な関数を呼ぼうとしても呼べちゃう....。
			//ptr->hoge();
			ptr = nullptr;
		}
	}
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

bool App::InitD3D()
{
	//デバイスの作成
	HRESULT hr = D3D12CreateDevice(
		nullptr,		//使用するデバイスを指定
		D3D_FEATURE_LEVEL_11_0,		//Direct3D11.0の機能をターゲット
		IID_PPV_ARGS(&m_pDevice));		//デバイスポインタのGUIDの取得とポインタの取得

	if (FAILED(hr))
	{
		return false;
	}

	//コマンドキューの作成
	{
		//コマンドリストをキューに入れたリスト及び、GPUに描画コマンドを送るメソッド提供を行う
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		//コマンドキューの優先度
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		//コマンドキューの特性を設定するフラグ
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
		if (FAILED(hr))
		{
			return false;
		}
	}

	//スワップチェーンの生成
	{
		//DXGIの作成
		IDXGIFactory4* pFactory = nullptr;
		//CreateDXGIFactory1()でFactor1を作るが、実際はFactor4以降のデータも対応しているので情報としては入っている
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
		if (FAILED(hr))
		{
			return false;
		}

		//スワップチェーンの設定
		DXGI_SWAP_CHAIN_DESC desc = {};
		//解像度の指定
		desc.BufferDesc.Width = m_Width;
		desc.BufferDesc.Height = m_Height;
		//リフレッシュレートの指定
		desc.BufferDesc.RefreshRate.Numerator = 60;		//分子
		desc.BufferDesc.RefreshRate.Denominator = 1;		//分母
		//表示するピクセルフォーマットの指定
		//RGBAそれぞれ8bitで、GPUに渡すときに0～1に正規化して渡す場合はDXGI_FORMAT_R8G8B8A8_UNORM
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//走査線の順序の指定. DXGI_MODE_SCANLINE_ORDER_UNSPECIFIEDは特に指定しない時に使用
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		//拡大縮小の設定. 特に考慮しない場合はDXGI_MODE_SCALING_UNSPECIFIED
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//マルチサンプリングの設定をここで指定
		desc.SampleDesc.Count = 1;		//マルチサンプリング数
		desc.SampleDesc.Quality = 0;		//品質レベル

		//スワップチェーンを作成するときに DXGI_USAGE_BACK_BUFFER を渡す必要はないらしいです
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//バックバッファの数
		desc.BufferCount = 2;
		//出力するウィンドウへのウィンドウハンドルを指定
		desc.OutputWindow = m_hWnd;
		//バックバッファ時の入れ替え時(Present関数を呼んだ時)の効果
		//D3D12アプリは、DXGI_SWAP_EFFECT_FLIP_SEQUENTIALまたはDXGI_SWAP_EFFECT_FLIP_DISCARDを使用する必要がある
		//DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL: Present関数を呼んだ後、バックバッファの内容が変化しない
		//DXGI_SWAP_EFFECT_FLIP_DISCARD: Present関数を呼んだ後、バックバッファの内容を破棄する
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//スワップチェイン動作オプション
		//DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH: ResizeTarget()を呼んで、表示サイズを切り替える
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGISwapChain* pSwapChain = nullptr;
		//IDXGISwapChainはIDXGISwapChain3の親クラス
		//pSwapChainはIDXGISwapChainのクラスだが、
		//CreateSwapChain()したとき、対応している子クラスの情報も入れられている
		hr = pFactory->CreateSwapChain(m_pQueue, &desc, &pSwapChain);
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		//CreateSwapChain()したとき、対応している子クラスの情報も入れられているので、
		//下記でIDXGISwapChain3のポインタが取得できる
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
		if (FAILED(hr))
		{
			SafeRelease(pSwapChain);
			SafeRelease(pFactory);
			return false;
		}

		//バックバッファ番号の取得. 描画先のindexを取得できた
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		SafeRelease(pSwapChain);
		SafeRelease(pFactory);

		//コマンドアロケーターの作成
		{
			for (auto i = 0u; i < FramCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(
					//コマンドアロケーターのタイプ
					D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPUが実行できるコマンドバッファを指定
					IID_PPV_ARGS(&m_pCmdAllocateor[i])
				);
			}
		}

		//コマンドリストの作成
		{
			hr = m_pDevice->CreateCommandList(
				0,		//GPUノードの識別. 1つしかGPUないので0でok
				//コマンドリストのタイプ
				D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPUが実行できるコマンドバッファを指定
				m_pCmdAllocateor[m_FrameIndex],		//コマンドアロケーターの指定
				nullptr,		//パイプラインステートの指定. 後で明示的に設定するのでここではnullptr
				IID_PPV_ARGS(&m_pCmdList)
			);

			if (FAILED(hr))
			{
				return false;
			}
		}

		//レンダーターゲットビューの作成
		{
			//リソースはスワップチェインがもつバッファなので作成不要
			//ディスクリプタヒープの生成.
			//ディスクリプタヒープとは、GPU上に作られるデスクリプタを保存するための配列
			//デスクリプタ: プロセスがOSを通じて入出力操作を行う際に参照される、ファイルの構造や内容の要約、ファイル属性などが記された整数

			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = FramCount;		//ヒープ内のディスクリプタ数
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//ヒープのタイプを指定
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		//ヒープの使用方法、参照の制限の決定
			desc.NodeMask = 0;		//GPUノードの指定

			//生成
			hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeapRTV));
			if (FAILED(hr))
			{
				return false;
			}

			//レンダーターゲットビュー(=描画先)の作成
			//ディスクリプションヒープの先頭のCPUディスクリプションハンドルを取得
			auto handle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
			//実態はディスクリプタのアドレスなので、2個以上取得するにはどれくらいずらせばよいかを求める必要がある。
			//ずらすためのサイズはGetDescriptorHandleIncrementSize()で取得可能. 
			//GetDescriptorHandleIncrementSize()は固定値だが「GPUによって値が変わる」ため注意
			auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			for (auto i = 0u; i < FramCount; i++)
			{
				//スワップチェインが持つバッファの取得. 引数はバックバッファ番号と格納先
				hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pColorBuffer[i]));
				if (FAILED(hr))
				{
					return false;
				}

				D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
				//画面で見るときのピクセルフォーマット. sRGBフォーマットを使用する際は、DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
				viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				//どのようにレンダーターゲットのリソースにアクセスするか
				//今回は二次元テクスチャを指定
				viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				/*
				* viewDesc.Texture2Dにテクスチャ2Dに関する設定を行う
				* 
				* 2次元テクスチャのアクセス設定(D3D12_TEX2D_RTV構造体)
				* typedef struct D3D12_TEX2D_RTV
				{
					UINT MipSlice;
					UINT PlaneSlice;
				} 	D3D12_TEX2D_RTV;
				*/
				//ミップマップレベルの番号(スワップチェイン作成時にミップマップレベルが1つしかないように設定したの0番を指定)
				viewDesc.Texture2D.MipSlice = 0;
				//使用するテクスチャの平面スライス番号(平面を複数枚持つデータではないので0を指定)
				viewDesc.Texture2D.MipSlice = 0;

				//レンダーターゲットビュー生成
				//リソース: m_pColorBuffer, 設定: viewDescのレンダーターゲットビューをhandleの位置handleに生成する
				m_pDevice->CreateRenderTargetView(m_pColorBuffer[i], &viewDesc, handle);

				//CPUディスクリプタのhandleを保存
				m_HandleRTV[i] = handle;
				//次のCPUディスクリプタのアドレスへ移動
				handle.ptr += incrementSize;
			}
		}
	}
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
