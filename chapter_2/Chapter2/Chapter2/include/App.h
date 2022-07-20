#pragma once
// pragma once��shader��#define�̂���������Ƒ����Ԃ݂����Ȃ��́B�������̂ق��������A�L�[���[�h���g���Ȃ��̂Ő��������

#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_4.h>
//�X�}�[�g�|�C���^���g����悤�ɂ���
#include <wrl/client.h>
//�v�Z���Zv�����C�u����
#include <DirectXMath.h>
#include <d3dcompiler.h>

//linker
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment( lib, "Winmm" )
#pragma comment( lib, "d3dcompiler.lib" )

//�^�w��̂Ƃ���Microsoft::WRL::ComPtr�Ƃ����̂𖈉񏑂��̂͐h���̂�using�G�C���A�X�Ŗ��O��Z��������悤�ɂ���
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

///////////////////////////////////////////////////////////////////////////////
// Transform structure
///////////////////////////////////////////////////////////////////////////////
//alignas(�l): �R���p�C���ɑ΂��ϐ�����������̓���̈ʒu�ɔz�u�i�A���C�����g�j����悤�ɗv������L�[���[�h
//�Ⴆ��4�o�C�g���E�̈ʒu�ɃA���C�����g�����ꍇ�A�ϐ���4�̔{���̃A�h���X�ɔz�u����A8�o�C�g���E�̈ʒu�ɃA���C�����g�����ꍇ�A�ϐ���8�̔{���̃A�h���X�ɔz�u�����B
struct alignas(256) Transform // ���ǉ�.
{
	DirectX::XMMATRIX   World;      // ���[���h�s��ł�.
	DirectX::XMMATRIX   View;       // �r���[�s��ł�.
	DirectX::XMMATRIX   Proj;       // �ˉe�s��ł�.
};

///////////////////////////////////////////////////////////////////////////////
// ConstantBufferView structure
///////////////////////////////////////////////////////////////////////////////
template<typename T>
struct ConstantBufferView // ���ǉ�.
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;               // �萔�o�b�t�@�̍\���ݒ�ł�.
	D3D12_CPU_DESCRIPTOR_HANDLE     HandleCPU;          // CPU�f�B�X�N���v�^�n���h���ł�.
	D3D12_GPU_DESCRIPTOR_HANDLE     HandleGPU;          // GPU�f�B�X�N���v�^�n���h���ł�.
	T* pBuffer;            // �o�b�t�@�擪�ւ̃|�C���^�ł�.
};

//���_�V�F�[�_�[�̓��͂ɍ��v���钸�_�f�[�^���쐬
//struct VSInput
//{
//	float3 position : POSITION;		//POSITION: ���W�f�[�^�ł��邱�Ƃ��`
//	float4 Color : COLOR;		//COLOR: ���_�J���[�f�[�^�ł��邱�Ƃ��`
//};
struct Vertex
{
	DirectX::XMFLOAT3 Position;		//�ʒu���
	DirectX::XMFLOAT4 Color;		//���_�J���[
};

class App
{
public:
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	//�t���[���o�b�t�@��
	static const uint32_t FrameCount = 2;

	HINSTANCE m_hInst;		//�C���X�^���X�n���h��
	HWND m_hWnd;			//�E�B���h�E�n���h��
	uint32_t m_Width;		//�E�B���h�E�̉���
	uint32_t m_Height;		//�E�B���h�E�̏c��

	ComPtr<ID3D12Device> m_pDevice;		//�f�o�C�X
	ComPtr<ID3D12CommandQueue> m_pQueue;		//�R�}���h�L���[
	ComPtr<IDXGISwapChain3> m_pSwapChain;		//�X���b�v�`�F�C��
	ComPtr<ID3D12Resource> m_pColorBuffer[FrameCount];		//�J���[�o�b�t�@
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocateor[FrameCount];		//�R�}���h�A���P�[�^�[
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;		//�R�}���h���X�g
	ComPtr<ID3D12DescriptorHeap> m_pHeapRTV;		//�f�B�X�N���v�^�q�[�v(�����_�[�^�[�Q�b�g�r���[)
	ComPtr<ID3D12Fence> m_pFence;		//�t�F���X
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		//�f�B�X�N���v�^�q�[�v(�萔�o�b�t�@�r���[�E�V�F�[�_���\�[�X�r���[�E�A���I�[�_�[�h�A�N�Z�X�r���[)
	ComPtr<ID3D12Resource> m_pVB;	//���_�o�b�t�@
	ComPtr<ID3D12Resource> m_pCB[FrameCount];		//�萔�o�b�t�@
	ComPtr<ID3D12Resource> m_pRootSignature;		//���[�g�V�O�j�`��
	ComPtr<ID3D12PipelineState> m_pSD;		//�p�C�v���C���X�e�[�g

	HANDLE m_pFenceEvent;		//�t�F���X�C�x���g
	uint64_t m_FenceCounter[FrameCount];		//�t�F���X�J�E���^�[
	uint32_t m_FrameIndex;		//�t���[���ԍ�
	D3D12_CPU_DESCRIPTOR_HANDLE m_HandleRTV[FrameCount];			//CPU�f�B�X�N���v�^�q�[�v(�����_�[�^�[�Q�b�g�r���[)
	D3D12_VERTEX_BUFFER_VIEW m_VBV;		//���_�o�b�t�@�r���[
	D3D12_VIEWPORT m_Viewport;		//�r���[�|�[�g
	D3D12_RECT m_Scissor;		//�V�U�[��`
	ConstantBufferView<Transform> m_CBV[FrameCount];		//�萔�o�b�t�@�r���[
	float m_RotateAngle;		//��]�p

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

