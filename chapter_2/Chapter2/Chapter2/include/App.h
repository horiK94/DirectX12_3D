#pragma once
// pragma onceはshaderの#defineのもうちょっと速い番みたいなもの。こっちのほうが速く、キーワードが使われないので推奨される

#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_4.h>
//スマートポインタを使えるようにする
#include <wrl/client.h>

//linker
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment( lib, "Winmm" )   // ★追加

//型指定のときにMicrosoft::WRL::ComPtrというのを毎回書くのは辛いのでusingエイリアスで名前を短くかけるようにする
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class App
{
public:
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	//フレームバッファ数
	static const uint32_t FrameCount = 2;

	HINSTANCE m_hInst;		//インスタンスハンドル
	HWND m_hWnd;			//ウィンドウハンドル
	uint32_t m_Width;		//ウィンドウの横幅
	uint32_t m_Height;		//ウィンドウの縦幅

	ComPtr<ID3D12Device> m_pDevice;		//デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue;		//コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain;		//スワップチェイン
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount];		//カラーバッファ
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocateor[FrameCount];		//コマンドアロケーター
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;		//コマンドリスト
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV;		//レンダーターゲットビュー
	ComPtr<ID3D12Fence> m_pFence;		//フェンス
	HANDLE m_pFenceEvent;		//フェンスイベント
	uint64_t m_FenceCounter[FrameCount];		//フェンスカウンター
	uint32_t m_FrameIndex;		//フレーム番号
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount];			//CPUディスクリプタヒープ(レンダーターゲットビュー)

	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();

	bool InitD3D();
	void TermD3D();
	void Render();
	void WaitGpu();
	void Present(uint32_t interval);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

