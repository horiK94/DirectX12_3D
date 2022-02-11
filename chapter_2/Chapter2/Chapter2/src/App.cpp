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
	//		//�K���Ȋ֐����Ăڂ��Ƃ��Ă��Ăׂ��Ⴄ....�B
	//		//ptr->hoge();
	//		ptr = nullptr;
	//	}
	//}
}
/// <summary>
/// �R���X�g���N�^
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

	//�A�v���P�[�V�����I������
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
	//�C���X�^���X�n���h���̎擾
	auto hInst = GetModuleHandle(nullptr);
	if (hInst == nullptr)
	{
		return false;
	}

	//�E�B���h�E�̐ݒ�
	WNDCLASSEX wc = {};
	//�\���̂̃T�C�Y
	wc.cbSize = sizeof(WNDCLASSEX);
	//�E�B���h�E�̃X�^�C�������߂�
	//CS_HREDRAW: ���������̃T�C�Y�ύX���ɃE�B���h�E�ĕ`��
	//CS_VREDRAW: ���������̃T�C�Y�ύX���ɃE�B���h�E�ĕ`��
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//�E�B���h�E�v���V�[�W�����̎w��
	wc.lpfnWndProc = WndProc;
	//�C���X�^���X�n���h���̐ݒ�. �Ȃ��Ă��悢���ۂ�
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(hInst, IDC_ARROW);
	//�E�B���h�E�̔w�i�F
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	//�E�B���h�E�N���X�����ʂ��閼�O
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION);

	//�E�B���h�E�̓o�^
	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	//�C���X�^���X�n���h���̐ݒ�
	m_hInst = hInst;

	//�E�B���h�E�T�C�Y�̐ݒ�
	RECT rt = {};
	rt.right = static_cast<LONG>(m_Width);
	rt.bottom = static_cast<LONG>(m_Height);

	//�쐬����E�B���h�E�̃X�^�C��. �ŏ����Őݒ肷��ق����A�������I�ȏ������s���Ă����
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	//�E�B���h�E�̐���
	m_hWnd = CreateWindowEx(
		0,
		//�E�B���h�E���ʖ�
		ClassName,
		TEXT("Test"),
		style,
		//�E�B���h�E�̉������̏����ʒu. ����̓f�t�H���g�l�w��
		CW_USEDEFAULT,
		//�E�B���h�E�̏c�����̏����ʒu. ����̓f�t�H���g�l�w��
		CW_USEDEFAULT,
		//�E�B���h�E��
		rt.right - rt.left,
		rt.bottom - rt.top,
		//�e�E�B���h�E�̎w��
		nullptr,
		//���j���[�A�q�E�B���h�E�̎w��
		nullptr,
		//�E�B���h�E�̃C���X�^���n���h��
		m_hInst,
		//�E�B���h�E�ɓn���p�����[�^
		nullptr);

	if (m_hWnd == nullptr)
	{
		//�����̎��s
		return false;
	}

	//�E�B���h�E�̕\��. ��2�����̓E�B���h�E�̕\���ݒ�
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	UpdateWindow(m_hWnd);

	//�E�B���h�E���t�H�[�J�X������
	SetFocus(m_hWnd);

	return true;
}

bool App::InitD3D()
{
	//�f�o�b�N���C���[�̗L����
#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> debug;
	auto hResult = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
	
	if (SUCCEEDED(hResult))
	{
		debug->EnableDebugLayer();
	}
#endif

	//�f�o�C�X�̍쐬
	HRESULT hr = D3D12CreateDevice(
		nullptr,		//�g�p����f�o�C�X���w��
		D3D_FEATURE_LEVEL_11_0,		//Direct3D11.0�̋@�\���^�[�Q�b�g
		IID_PPV_ARGS(m_pDevice.GetAddressOf()));		//�f�o�C�X�|�C���^��GUID�̎擾�ƃ|�C���^�̎擾

	if (FAILED(hr))
	{
		return false;
	}

	//�R�}���h�L���[�̍쐬
	{
		//�R�}���h���X�g���L���[�ɓ��ꂽ���X�g�y�сAGPU�ɕ`��R�}���h�𑗂郁�\�b�h�񋟂��s��
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		//�R�}���h�L���[�̗D��x
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		//�R�}���h�L���[�̓�����ݒ肷��t���O
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}
	}

	//�X���b�v�`�F�[���̐���
	{
		//DXGI�̍쐬
		ComPtr<IDXGIFactory4> pFactory = nullptr;
		//CreateDXGIFactory1()��Factor1����邪�A���ۂ�Factor4�ȍ~�̃f�[�^���Ή����Ă���̂ŏ��Ƃ��Ă͓����Ă���
		hr = CreateDXGIFactory1(IID_PPV_ARGS(pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		//�X���b�v�`�F�[���̐ݒ�
		DXGI_SWAP_CHAIN_DESC desc = {};
		//�𑜓x�̎w��
		desc.BufferDesc.Width = m_Width;
		desc.BufferDesc.Height = m_Height;
		//���t���b�V�����[�g�̎w��
		desc.BufferDesc.RefreshRate.Numerator = 60;		//���q
		desc.BufferDesc.RefreshRate.Denominator = 1;		//����
		//�\������s�N�Z���t�H�[�}�b�g�̎w��
		//RGBA���ꂼ��8bit�ŁAGPU�ɓn���Ƃ���0�`1�ɐ��K�����ēn���ꍇ��DXGI_FORMAT_R8G8B8A8_UNORM
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//�������̏����̎w��. DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED�͓��Ɏw�肵�Ȃ����Ɏg�p
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		//�g��k���̐ݒ�. ���ɍl�����Ȃ��ꍇ��DXGI_MODE_SCALING_UNSPECIFIED
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//�}���`�T���v�����O�̐ݒ�������Ŏw��
		desc.SampleDesc.Count = 1;		//�}���`�T���v�����O��
		desc.SampleDesc.Quality = 0;		//�i�����x��

		//�X���b�v�`�F�[�����쐬����Ƃ��� DXGI_USAGE_BACK_BUFFER ��n���K�v�͂Ȃ��炵���ł�
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//�o�b�N�o�b�t�@�̐�
		desc.BufferCount = FrameCount;
		//�o�͂���E�B���h�E�ւ̃E�B���h�E�n���h�����w��
		desc.OutputWindow = m_hWnd;
		//Window�\�������邩. �L�ڂ��Ȃ���FALSE���ƑS��ʂɂȂ�
		desc.Windowed = TRUE;
		//�o�b�N�o�b�t�@���̓���ւ���(Present�֐����Ă񂾎�)�̌���
		//D3D12�A�v���́ADXGI_SWAP_EFFECT_FLIP_SEQUENTIAL�܂���DXGI_SWAP_EFFECT_FLIP_DISCARD���g�p����K�v������
		//DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL: Present�֐����Ă񂾌�A�o�b�N�o�b�t�@�̓��e���ω����Ȃ�
		//DXGI_SWAP_EFFECT_FLIP_DISCARD: Present�֐����Ă񂾌�A�o�b�N�o�b�t�@�̓��e��j������
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//�X���b�v�`�F�C������I�v�V����
		//DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH: ResizeTarget()���Ă�ŁA�\���T�C�Y��؂�ւ���
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ComPtr<IDXGISwapChain> pSwapChain = nullptr;
		//IDXGISwapChain��IDXGISwapChain3�̐e�N���X
		//pSwapChain��IDXGISwapChain�̃N���X�����A
		//CreateSwapChain()�����Ƃ��A�Ή����Ă���q�N���X�̏���������Ă���
		hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);
		if (FAILED(hr))
		{
			return false;
		}

		//CreateSwapChain()�����Ƃ��A�Ή����Ă���q�N���X�̏���������Ă���̂ŁA
		//���L��IDXGISwapChain3�̃|�C���^���擾�ł���
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.GetAddressOf()));
		if (FAILED(hr))
		{
			pSwapChain;
			return false;
		}

		//�o�b�N�o�b�t�@�ԍ��̎擾. �`����index���擾�ł���
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		pSwapChain.Reset();

		//�R�}���h�A���P�[�^�[�̍쐬
		{
			for (auto i = 0u; i < FrameCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(
					//�R�}���h�A���P�[�^�[�̃^�C�v
					D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPU�����s�ł���R�}���h�o�b�t�@���w��
					IID_PPV_ARGS(m_pCmdAllocateor[i].GetAddressOf())
				);
			}
		}

		//�R�}���h���X�g�̍쐬
		{
			hr = m_pDevice->CreateCommandList(
				0,		//GPU�m�[�h�̎���. 1����GPU�Ȃ��̂�0��ok
				//�R�}���h���X�g�̃^�C�v
				D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPU�����s�ł���R�}���h�o�b�t�@���w��
				m_pCmdAllocateor[m_FrameIndex].Get(),		//�R�}���h�A���P�[�^�[�̎w��
				nullptr,		//�p�C�v���C���X�e�[�g�̎w��. ��Ŗ����I�ɐݒ肷��̂ł����ł�nullptr
				IID_PPV_ARGS(m_pCmdList.GetAddressOf())
			);

			if (FAILED(hr))
			{
				return false;
			}
		}

		//�����_�[�^�[�Q�b�g�r���[�̍쐬
		{
			//���\�[�X�̓X���b�v�`�F�C�������o�b�t�@�Ȃ̂ō쐬�s�v
			//�f�B�X�N���v�^�q�[�v�̐���.
			//�f�B�X�N���v�^�q�[�v�Ƃ́AGPU��ɍ����f�X�N���v�^��ۑ����邽�߂̔z��
			//�f�X�N���v�^: �v���Z�X��OS��ʂ��ē��o�͑�����s���ۂɎQ�Ƃ����A�t�@�C���̍\������e�̗v��A�t�@�C�������Ȃǂ��L���ꂽ����

			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.NumDescriptors = FrameCount;		//�q�[�v���̃f�B�X�N���v�^��
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//�q�[�v�̃^�C�v���w��
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		//�q�[�v�̎g�p���@�A�Q�Ƃ̐����̌���
			desc.NodeMask = 0;		//GPU�m�[�h�̎w��

			//����
			hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapRTV.GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}

			//�����_�[�^�[�Q�b�g�r���[(=�`���)�̍쐬
			//�f�B�X�N���v�V�����q�[�v�̐擪��CPU�f�B�X�N���v�V�����n���h�����擾
			auto handle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
			//���Ԃ̓f�B�X�N���v�^�̃A�h���X�Ȃ̂ŁA2�ȏ�擾����ɂ͂ǂꂭ�炢���点�΂悢�������߂�K�v������B
			//���炷���߂̃T�C�Y��GetDescriptorHandleIncrementSize()�Ŏ擾�\. 
			//GetDescriptorHandleIncrementSize()�͌Œ�l�����uGPU�ɂ���Ēl���ς��v���ߒ���
			auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			for (auto i = 0u; i < FrameCount; i++)
			{
				//�X���b�v�`�F�C�������o�b�t�@�̎擾. �����̓o�b�N�o�b�t�@�ԍ��Ɗi�[��
				hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pColorBuffer[i].GetAddressOf()));
				if (FAILED(hr))
				{
					return false;
				}

				D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
				//��ʂŌ���Ƃ��̃s�N�Z���t�H�[�}�b�g. sRGB�t�H�[�}�b�g���g�p����ۂ́ADXGI_FORMAT_R8G8B8A8_UNORM_SRGB
				viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				//�ǂ̂悤�Ƀ����_�[�^�[�Q�b�g�̃��\�[�X�ɃA�N�Z�X���邩
				//����͓񎟌��e�N�X�`�����w��
				viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				/*
				* viewDesc.Texture2D�Ƀe�N�X�`��2D�Ɋւ���ݒ���s��
				*
				* 2�����e�N�X�`���̃A�N�Z�X�ݒ�(D3D12_TEX2D_RTV�\����)
				* typedef struct D3D12_TEX2D_RTV
				{
					UINT MipSlice;
					UINT PlaneSlice;
				} 	D3D12_TEX2D_RTV;
				*/
				//�~�b�v�}�b�v���x���̔ԍ�(�X���b�v�`�F�C���쐬���Ƀ~�b�v�}�b�v���x����1�����Ȃ��悤�ɐݒ肵���̂�0�Ԃ��w��)
				viewDesc.Texture2D.MipSlice = 0;
				//�g�p����e�N�X�`���̕��ʃX���C�X�ԍ�(���ʂ𕡐������f�[�^�ł͂Ȃ��̂�0���w��)
				viewDesc.Texture2D.MipSlice = 0;

				//�����_�[�^�[�Q�b�g�r���[����
				//���\�[�X: m_pColorBuffer, �ݒ�: viewDesc�̃����_�[�^�[�Q�b�g�r���[��handle�̈ʒuhandle�ɐ�������
				m_pDevice->CreateRenderTargetView(m_pColorBuffer[i].Get(), &viewDesc, handle);

				//CPU�f�B�X�N���v�^��handle��ۑ�
				m_HandleRTV[i] = handle;
				//����CPU�f�B�X�N���v�^�̃A�h���X�ֈړ�
				handle.ptr += incrementSize;
			}
		}
	}

	{
		//�t�F���X�̐���
		//�`�抮����������l�Ŕ��f���邽�߂̂���. ���g�͐����l�̕ϐ��Ƃقڈꏏ
		hr = m_pDevice->CreateFence(m_FenceCounter[m_FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		//�t�F���X�̒l���C���N�������g���ꂽ��`�揈���������������ƂɂȂ邪�A
		//���ۂ܂ŏI���܂ő҂����̓C�x���g�Ŏ�������. while()�ł����ƊĎ����邱�Ƃ��\�����A�������OS�ɑ҂̂�C�����ق��������I�ɃR���p�C�������
		m_pFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_pFence == nullptr)
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// �`�揈��
/// </summary>
void App::Render()
{
	//�R�}���h�A���P�[�^�[��Reset()���ĂсA�R�}���h�o�b�t�@�̓�����擪�ɖ߂�
	//�R�}���h�A���P�[�^�[�ɃR�}���h���X�g���ς܂��ƁAGPU�������ł���悤�Ȗ��߂ɕϊ�����A�O���t�B�b�N�X�������ɐς܂��B
	//���̌��ʁAReset()���ĐςނƃO���t�B�b�N�X�������̖��߂��㏑�����Ă��܂�����s����ɂ����Ă��܂��̂ŁA�R�}���h���X�g��GPU�Ŏ��s���I����܂�Reset()���Ă�ł͂����Ȃ�
	//�܂�A���̊֐����Ă΂��܂ł�GPU�̏������I���Ă���K�v������
	m_pCmdAllocateor[m_FrameIndex]->Reset();
	//�R�}���h���X�g�̒��g�����Z�b�g���A�`��R�}���h��������悤�ɂ���
	m_pCmdList->Reset(m_pCmdAllocateor[m_FrameIndex].Get(), nullptr);

	//�������݂̂��߂̃��\�[�X�o���A
	D3D12_RESOURCE_BARRIER barrier = {};
	//�g�p�p�r�̏�Ԃ̑J�ڂ��s����̂ŁAD3D12_RESOURCE_BARRIER_TYPE_TRANSITION���w��
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;		//�t���O�͓��Ɏw��Ȃ�
	//D3D12_RESOURCE_BARRIER_TYPE_TRANSITION�w�莞�� Transition�\���̂Ƀf�[�^�����Ă���
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();		//�J�ڂɎg�p���郊�\�[�X�̃|�C���^
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			//D3D12_RESOURCE_STATE_COMMON�Ɠ���. (�X���b�v�`�F�[���o�b�N�o�b�t�@�[��D3D12_RESOURCE_STATE_COMMON ��ԂŎ����I�ɊJ�n�����Ƃ̂���)
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		//�����_�[�^�[�Q�b�g�Ƃ��Ďg�p����郊�\�[�X�ɂȂ�
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;		//�J�ڂ̂��߂̃T�u���\�[�X�̔ԍ��w��. �S�ẴT�u���\�[�X��J�ڂ�����ɂ�D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES���w��

	m_pCmdList->ResourceBarrier(1, &barrier);

	//�����_�[�^�[�Q�b�g�r���[��`��f�o�C�X�ɃZ�b�g
	//��1�����͂����ݒ肷�邩
	//��2�����͐ݒ肵���������_�[�^�[�Q�b�g�̃f�B�X�N���v�^
	m_pCmdList->OMSetRenderTargets(1, &m_HandleRTV[m_FrameIndex], FALSE, nullptr);

	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	m_pCmdList->ClearRenderTargetView(m_HandleRTV[m_FrameIndex], clearColor, 0, nullptr);

	//�����_�[�^�[�Q�b�g�ɏ������ޕK�v���Ȃ��Ȃ����̂ŁA�����_�[�^�[�Q�b�g���u�\������v��Ԃɖ߂�
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//�R�}���h�̋L�^�I��
	m_pCmdList->Close();

	//�R�}���h�̎��s
	ID3D12CommandList* ppCmdList[] = { m_pCmdList.Get() };
	//�R�}���h�L���[�ɓ����Ď��s
	m_pQueue->ExecuteCommandLists(1, ppCmdList);

	Present(1);
}

void App::Present(uint32_t interval)
{
	//Present(): �t�����g�o�b�t�@����ʂɕ\��. �o�b�N�o�b�t�@���X���b�v����
	//��1����: �f�B�X�v���C�̐��������ƃt���[���̓������@���w��
	//0�� �����ɕ\��
	//1�� 1�񐂒�������҂�. �f�B�X�v���C�̍X�V������60Hz�Ȃ�60FPS
	//2�� 2�񐂒�������҂�. �f�B�X�v���C�̍X�V������60Hz�Ȃ�30FPS
	m_pSwapChain->Present(interval, 0);

	//�R�}���h�L���[�����s���ꂽ�Ƃ��Ƀt�F���X�̒l���X�V�����悤�ɂ���
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	//�ȑO�Z�b�g�����R�}���h��GPU��ł̎��s���������Ă���������͊��������Ƃ��ɁA��1�����̒l���2�����̒l�ɍX�V����
	m_pQueue->Signal(m_pFence.Get(), currentValue);

	//�o�b�N�o�b�t�@�̒l���擾
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	//GetCompletedValue(): �t�F���X�̃J�E���^�l���擾
	//cout << m_pFence->GetCompletedValue() << ", " << m_FenceCounter[0] << ", " << m_FenceCounter[1] << "frame" << m_FrameIndex  << " , cur: " << currentValue << endl;
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		//ID3D12Fence::SetEventOnCompletion()���g���ƁAfence value����1�����̒l�ɂȂ����Ƃ��A��2�����̃C�x���g�I�u�W�F�N�g���V�O�i����ԂɑJ�ڂ����܂�
		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_pFenceEvent);
		//WaitForSingleObjectEx()�ŃX���b�h��Q������΁AGPU�̎��s������ɒ@���N�����Ă���܂�
		WaitForSingleObjectEx(m_pFenceEvent, INFINITE, FALSE);
		//cout << "wait" << m_pFence->GetCompletedValue() << ", " << m_FenceCounter[0] << ", " << m_FenceCounter[1] << endl;

	}
	//cout << "�Ă΂ꂽ!" << endl;
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

void App::WaitGpu()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_pFenceEvent != nullptr);

	//�ȑO�Z�b�g�����R�}���h��GPU��ł̎��s���������Ă���������͊��������Ƃ��ɁA��1�����̒l���2�����̒l�ɍX�V����
	m_pQueue->Signal(m_pFence.Get(), m_FenceCounter[m_FrameIndex]);

	//�������ɃC�x���g��ݒ�
	//ID3D12Fence::SetEventOnCompletion()���g���ƁAfence value����1�����̒l�ɂȂ����Ƃ��A��2�����̃C�x���g�I�u�W�F�N�g���V�O�i����ԂɑJ�ڂ����܂�
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_pFenceEvent);

	//WaitForSingleObjectEx()�ŃX���b�h��Q������. ��P�����̃C�x���g�I�u�W�F�N�g���V�O�i����ԂɑJ�ڂ�����ҋ@���I������
	WaitForSingleObjectEx(m_pFenceEvent, INFINITE, FALSE);

	m_FenceCounter[m_FrameIndex]++;
}

void App::TermD3D()
{
	//GPU�̏������I���̂�҂�
	WaitGpu();

	//�C�x���g�j��
	if (m_pFenceEvent != nullptr)
	{
		CloseHandle(m_pFenceEvent);
		m_pFenceEvent = nullptr;
	}

	//�X�}�[�g�|�C���^�[�̖����I�ȉ��

	//�t�F���X�j��
	m_pFence.Reset();

	//�����_�[�^�[�Q�b�g�r���[�̍폜
	m_pHeapRTV.Reset();
	for (int i = 0; i < FrameCount; ++i)
	{
		m_pColorBuffer[i].Reset();
	}

	//�R�}���h���X�g�̍폜
	m_pCmdList.Reset();

	//�R�}���h�A���P�[�^�[�̔j��
	for (int i = 0; i < FrameCount; i++)
	{
		m_pCmdAllocateor[i].Reset();
	}

	//�X���b�v�`�F�C���̍폜
	m_pSwapChain.Reset();

	//�R�}���h�L���[�̍폜
	m_pQueue.Reset();

	//�f�o�C�X�̍폜
	m_pDevice.Reset();
}

//�E�B���h�E�v���V�[�W��
//WindowsOS���痈�����b�Z�[�W�L���[���ʒm�����֐�
LRESULT CALLBACK App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:		//�E�B���h�E�j��
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
		//�A�v���P�[�V�������Ƀ��b�Z�[�W�������Ă��邩. �����Ă����msg�ɑ�������
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
		{
			TranslateMessage(&msg);		//�L�[�{�[�h�̓��͂����ۂ̕����ɕϊ�
			DispatchMessage(&msg);		//�E�B���h�E�v���V�[�W�����ɑ���
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

	//�E�B���h�E�̏I������
	TermWnd();
}

void App::TermWnd()
{
	//�E�B���h�E�̓o�^������
	if (m_hInst != nullptr)
	{
		UnregisterClass(ClassName, m_hInst);
	}

	m_hInst = nullptr;
	m_hWnd = nullptr;
}