#pragma once
// pragma onceはshaderの#defineのもうちょっと速い番みたいなもの。こっちのほうが速く、キーワードが使われないので推奨される

#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_4.h>
//スマートポインタを使えるようにする
#include <wrl/client.h>
//計算演算vｂライブラリ
#include <DirectXMath.h>
#include <d3dcompiler.h>

//linker
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment( lib, "Winmm" )
#pragma comment( lib, "d3dcompiler.lib" )

//型指定のときにMicrosoft::WRL::ComPtrというのを毎回書くのは辛いのでusingエイリアスで名前を短くかけるようにする
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

///////////////////////////////////////////////////////////////////////////////
// Transform structure
///////////////////////////////////////////////////////////////////////////////
//alignas(値): コンパイラに対し変数をメモリ上の特定の位置に配置（アライメント）するように要求するキーワード
//例えば4バイト境界の位置にアライメントした場合、変数は4の倍数のアドレスに配置され、8バイト境界の位置にアライメントした場合、変数は8の倍数のアドレスに配置される。
struct alignas(256) Transform // ★追加.
{
	DirectX::XMMATRIX   World;      // ワールド行列です.
	DirectX::XMMATRIX   View;       // ビュー行列です.
	DirectX::XMMATRIX   Proj;       // 射影行列です.
};

///////////////////////////////////////////////////////////////////////////////
// ConstantBufferView structure
///////////////////////////////////////////////////////////////////////////////
template<typename T>
struct ConstantBufferView // ★追加.
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;               // 定数バッファの構成設定です.
	D3D12_CPU_DESCRIPTOR_HANDLE     HandleCPU;          // CPUディスクリプタハンドルです.
	D3D12_GPU_DESCRIPTOR_HANDLE     HandleGPU;          // GPUディスクリプタハンドルです.
	T* pBuffer;            // バッファ先頭へのポインタです.
};

//頂点シェーダーの入力に合致する頂点データを作成
//struct VSInput
//{
//	float3 position : POSITION;		//POSITION: 座標データであることを定義
//	float4 Color : COLOR;		//COLOR: 頂点カラーデータであることを定義
//};
struct Vertex
{
	DirectX::XMFLOAT3 Position;		//位置情報
	DirectX::XMFLOAT4 Color;		//頂点カラー
};

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
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV;		//ディスクリプタヒープ(レンダーターゲットビュー)
	ComPtr<ID3D12Fence> m_pFence;		//フェンス
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		//ディスクリプタヒープ(定数バッファビュー・シェーダリソースビュー・アンオーダードアクセスビュー)
	ComPtr<ID3D12Resource> m_pVB;	//頂点バッファ
	ComPtr<ID3D12Resource> m_pCB[FrameCount];		//定数バッファ
	ComPtr<ID3D12Resource> m_pRootSignature;		//ルートシグニチャ
	ComPtr<ID3D12PipelineState> m_pSD;		//パイプラインステート

	HANDLE m_pFenceEvent;		//フェンスイベント
	uint64_t m_FenceCounter[FrameCount];		//フェンスカウンター
	uint32_t m_FrameIndex;		//フレーム番号
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount];			//CPUディスクリプタヒープ(レンダーターゲットビュー)
	D3D12_VERTEX_BUFFER_VIEW m_VBV;		//頂点バッファビュー
	D3D12_VIEWPORT m_Viewport;		//ビューポート
	D3D12_RECT m_Scissor;		//シザー矩形
	ConstantBufferView<Transform> m_CBV[FrameCount];		//定数バッファビュー
	float m_RotateAngle;		//回転角

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

	bool OnInit();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

