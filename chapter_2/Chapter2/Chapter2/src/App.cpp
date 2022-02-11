#include "App.h"
#include <cassert>
#include <iostream>
#include <cmath>
using namespace std;

namespace
{
	const auto ClassName = TEXT("SampleWindowClass");

	//template<typename T>
	//void SafeRelease(T*& ptr)
	//{
	//	if (ptr != nullptr)
	//	{
	//		ptr->Release();
	//		//適当な関数を呼ぼうとしても呼べちゃう....。
	//		//ptr->hoge();
	//		ptr = nullptr;
	//	}
	//}
}
/// <summary>
/// コンストラクタ
/// </summary>
App::App(uint32_t width, uint32_t height)
	: m_hInst(nullptr)
	, m_hWnd(nullptr)
	, m_Width(width)
	, m_Height(height)
	, m_pDevice(nullptr)
	, m_pQueue(nullptr)
	, m_pSwapChain(nullptr)
	, m_pCmdList(nullptr)
	, m_pHeapRTV(nullptr)
	, m_pFence(nullptr)
	, m_FrameIndex(0) 
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
	if (!InitWnd())
	{
		return false;
	}

	return InitD3D();
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

	UpdateWindow(m_hWnd);

	//ウィンドウをフォーカスさせる
	SetFocus(m_hWnd);

	return true;
}

bool App::InitD3D()
{
	//デバックレイヤーの有効化
#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> debug;
	auto hResult = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
	
	if (SUCCEEDED(hResult))
	{
		debug->EnableDebugLayer();
	}
#endif

	//デバイスの作成
	HRESULT hr = D3D12CreateDevice(
		nullptr,		//使用するデバイスを指定
		D3D_FEATURE_LEVEL_11_0,		//Direct3D11.0の機能をターゲット
		IID_PPV_ARGS(m_pDevice.GetAddressOf()));		//デバイスポインタのGUIDの取得とポインタの取得

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

		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}
	}

	//スワップチェーンの生成
	{
		//DXGIの作成
		ComPtr<IDXGIFactory4> pFactory = nullptr;
		//CreateDXGIFactory1()でFactor1を作るが、実際はFactor4以降のデータも対応しているので情報としては入っている
		hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
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
		desc.BufferCount = FrameCount;
		//出力するウィンドウへのウィンドウハンドルを指定
		desc.OutputWindow = m_hWnd;
		//Window表示をするか. 記載しないかFALSEだと全画面になる
		desc.Windowed = TRUE;
		//バックバッファ時の入れ替え時(Present関数を呼んだ時)の効果
		//D3D12アプリは、DXGI_SWAP_EFFECT_FLIP_SEQUENTIALまたはDXGI_SWAP_EFFECT_FLIP_DISCARDを使用する必要がある
		//DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL: Present関数を呼んだ後、バックバッファの内容が変化しない
		//DXGI_SWAP_EFFECT_FLIP_DISCARD: Present関数を呼んだ後、バックバッファの内容を破棄する
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//スワップチェイン動作オプション
		//DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH: ResizeTarget()を呼んで、表示サイズを切り替える
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain> pSwapChain = nullptr;
		//IDXGISwapChainはIDXGISwapChain3の親クラス
		//pSwapChainはIDXGISwapChainのクラスだが、
		//CreateSwapChain()したとき、対応している子クラスの情報も入れられている
		hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);
		if (FAILED(hr))
		{
			return false;
		}

		//CreateSwapChain()したとき、対応している子クラスの情報も入れられているので、
		//下記でIDXGISwapChain3のポインタが取得できる
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.GetAddressOf()));
		if (FAILED(hr))
		{
			pSwapChain;
			return false;
		}

		//バックバッファ番号の取得. 描画先のindexを取得できた
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		pSwapChain.Reset();

		//コマンドアロケーターの作成
		{
			for (auto i = 0u; i < FrameCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(
					//コマンドアロケーターのタイプ
					D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPUが実行できるコマンドバッファを指定
					IID_PPV_ARGS(m_pCmdAllocateor[i].GetAddressOf())
				);
			}
		}

		//コマンドリストの作成
		{
			hr = m_pDevice->CreateCommandList(
				0,		//GPUノードの識別. 1つしかGPUないので0でok
				//コマンドリストのタイプ
				D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPUが実行できるコマンドバッファを指定
				m_pCmdAllocateor[m_FrameIndex].Get(),		//コマンドアロケーターの指定
				nullptr,		//パイプラインステートの指定. 後で明示的に設定するのでここではnullptr
				IID_PPV_ARGS(m_pCmdList.GetAddressOf())
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
			desc.NumDescriptors = FrameCount;		//ヒープ内のディスクリプタ数
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//ヒープのタイプを指定
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		//ヒープの使用方法、参照の制限の決定
			desc.NodeMask = 0;		//GPUノードの指定

			//生成
			hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapRTV.GetAddressOf()));
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

			for (auto i = 0u; i < FrameCount; i++)
			{
				//スワップチェインが持つバッファの取得. 引数はバックバッファ番号と格納先
				hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].GetAddressOf()));
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
				//ミップマップレベルの番号(スワップチェイン作成時にミップマップレベルが1つしかないように設定したので0番を指定)
				viewDesc.Texture2D.MipSlice = 0;
				//使用するテクスチャの平面スライス番号(平面を複数枚持つデータではないので0を指定)
				viewDesc.Texture2D.MipSlice = 0;

				//レンダーターゲットビュー生成
				//リソース: m_pColorBuffer, 設定: viewDescのレンダーターゲットビューをhandleの位置handleに生成する
				m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);

				//CPUディスクリプタのhandleを保存
				m_HandleRTV[i] = handle;
				//次のCPUディスクリプタのアドレスへ移動
				handle.ptr += incrementSize;
			}
		}
	}

	{
		//フェンスの生成
		//描画完了したかを値で判断するためのもの. 中身は整数値の変数とほぼ一緒
		hr = m_pDevice->CreateFence(m_FenceCounter[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		//フェンスの値がインクリメントされたら描画処理が完了したことになるが、
		//実際まで終わるまで待つ処理はイベントで実装する. while()でずっと監視することも可能だが、それよりもOSに待つのを任せたほうが効率的にコンパイルされる
		m_pFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_pFence == nullptr)
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// 描画処理
/// </summary>
void App::Render()
{
	//コマンドアロケーターのReset()を呼び、コマンドバッファの入れ先を先頭に戻す
	//コマンドアロケーターにコマンドリストが積まれると、GPUが理解できるような命令に変換され、グラフィックスメモリに積まれる。
	//その結果、Reset()して積むとグラフィックスメモリの命令を上書きしてしまい動作不安定にさせてしまうので、コマンドリストがGPUで実行を終えるまでReset()を呼んではいけない
	//つまり、この関数が呼ばれるまでにGPUの処理を終えている必要がある
	m_pCmdAllocateor[m_FrameIndex]->Reset();
	//コマンドリストの中身をリセットし、描画コマンドを入れられるようにする
	m_pCmdList->Reset(m_pCmdAllocateor[m_FrameIndex].Get(), nullptr);

	//書き込みのためのリソースバリア
	D3D12_RESOURCE_BARRIER barrier = {};
	//使用用途の状態の遷移が行われるので、D3D12_RESOURCE_BARRIER_TYPE_TRANSITIONを指定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		//フラグは特に指定なし
	//D3D12_RESOURCE_BARRIER_TYPE_TRANSITION指定時は Transition構造体にデータを入れていく
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();		//遷移に使用するリソースのポインタ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			//D3D12_RESOURCE_STATE_COMMONと同じ. (スワップチェーンバックバッファーはD3D12_RESOURCE_STATE_COMMON 状態で自動的に開始されるとのこと)
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		//レンダーターゲットとして使用されるリソースになる
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;		//遷移のためのサブリソースの番号指定. 全てのサブリソースを遷移させるにはD3D12_RESOURCE_BARRIER_ALL_SUBRESOURCESを指定

	m_pCmdList->ResourceBarrier(1, &barrier);

	//レンダーターゲットビューを描画デバイスにセット
	//第1引数はいくつ設定するか
	//第2引数は設定したいレンダーターゲットのディスクリプタ
	m_pCmdList->OMSetRenderTargets(1, &m_HandleRTV[m_FrameIndex], FALSE, nullptr);

	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	m_pCmdList->ClearRenderTargetView(m_HandleRTV[m_FrameIndex], clearColor, 0, nullptr);

	//レンダーターゲットに書き込む必要がなくなったので、レンダーターゲットを「表示する」状態に戻す
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//コマンドの記録終了
	m_pCmdList->Close();

	//コマンドの実行
	ID3D12CommandList* ppCmdList[] = { m_pCmdList.Get() };
	//コマンドキューに投げて実行
	m_pQueue->ExecuteCommandLists(1, ppCmdList);

	Present(1);
}

void App::Present(uint32_t interval)
{
	//Present(): フロントバッファを画面に表示. バックバッファをスワップする
	//第1引数: ディスプレイの垂直同期とフレームの同期方法を指定
	//0→ 即座に表示
	//1→ 1回垂直同期を待つ. ディスプレイの更新周期が60Hzなら60FPS
	//2→ 2回垂直同期を待つ. ディスプレイの更新周期が60Hzなら30FPS
	m_pSwapChain->Present(interval, 0);

	//コマンドキューが実行されたときにフェンスの値が更新されるようにする
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	//以前セットしたコマンドがGPU上での実行を完了しているもしくは完了したときに、第1引数の値を第2引数の値に更新する
	m_pQueue->Signal(m_pFence.Get(), currentValue);

	//バックバッファの値を取得
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	//GetCompletedValue(): フェンスのカウンタ値を取得
	//cout << m_pFence->GetCompletedValue() << ", " << m_FenceCounter[0] << ", " << m_FenceCounter[1] << "frame" << m_FrameIndex  << " , cur: " << currentValue << endl;
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		//ID3D12Fence::SetEventOnCompletion()を使うと、fence valueが第1引数の値になったとき、第2引数のイベントオブジェクトをシグナル状態に遷移させます
		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_pFenceEvent);
		//WaitForSingleObjectEx()でスレッドを寝かせれば、GPUの実行完了後に叩き起こしてくれます
		WaitForSingleObjectEx(m_pFenceEvent, INFINITE, FALSE);
		//cout << "wait" << m_pFence->GetCompletedValue() << ", " << m_FenceCounter[0] << ", " << m_FenceCounter[1] << endl;

	}
	//cout << "呼ばれた!" << endl;
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

void App::WaitGpu()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_pFenceEvent != nullptr);

	//以前セットしたコマンドがGPU上での実行を完了しているもしくは完了したときに、第1引数の値を第2引数の値に更新する
	m_pQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	//完了時にイベントを設定
	//ID3D12Fence::SetEventOnCompletion()を使うと、fence valueが第1引数の値になったとき、第2引数のイベントオブジェクトをシグナル状態に遷移させます
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_pFenceEvent);

	//WaitForSingleObjectEx()でスレッドを寝かせる. 第１引数のイベントオブジェクトがシグナル状態に遷移したら待機を終了する
	WaitForSingleObjectEx(m_pFenceEvent, INFINITE, FALSE);

	m_FenceCounter[m_FrameIndex]++;
}

void App::TermD3D()
{
	//GPUの処理が終わるのを待つ
	WaitGpu();

	//イベント破棄
	if (m_pFenceEvent != nullptr)
	{
		CloseHandle(m_pFenceEvent);
		m_pFenceEvent = nullptr;
	}

	//スマートポインターの明示的な解放

	//フェンス破棄
	m_pFence.Reset();

	//レンダーターゲットビューの削除
	m_pHeapRTV.Reset();
	for (int i = 0; i < FrameCount; ++i)
	{
		m_pColorBuffer[i].Reset();
	}

	//コマンドリストの削除
	m_pCmdList.Reset();

	//コマンドアロケーターの破棄
	for (int i = 0; i < FrameCount; i++)
	{
		m_pCmdAllocateor[i].Reset();
	}

	//スワップチェインの削除
	m_pSwapChain.Reset();

	//コマンドキューの削除
	m_pQueue.Reset();

	//デバイスの削除
	m_pDevice.Reset();
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
		else
		{
			Render();
		}
	}
}

void App::TermApp()
{
	TermD3D();

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