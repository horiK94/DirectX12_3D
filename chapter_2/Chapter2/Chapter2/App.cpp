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
			//�K���Ȋ֐����Ăڂ��Ƃ��Ă��Ăׂ��Ⴄ....�B
			//ptr->hoge();
			ptr = nullptr;
		}
	}
}
/// <summary>
/// �R���X�g���N�^
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

	//�A�v���P�[�V�����I������
	TermApp();
}

bool App::InitApp()
{
	return InitWnd();
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

	//�E�B���h�E���t�H�[�J�X������
	SetFocus(m_hWnd);

	return true;
}

bool App::InitD3D()
{
	//�f�o�C�X�̍쐬
	HRESULT hr = D3D12CreateDevice(
		nullptr,		//�g�p����f�o�C�X���w��
		D3D_FEATURE_LEVEL_11_0,		//Direct3D11.0�̋@�\���^�[�Q�b�g
		IID_PPV_ARGS(&m_pDevice));		//�f�o�C�X�|�C���^��GUID�̎擾�ƃ|�C���^�̎擾

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

		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
		if (FAILED(hr))
		{
			return false;
		}
	}

	//�X���b�v�`�F�[���̐���
	{
		//DXGI�̍쐬
		IDXGIFactory4* pFactory = nullptr;
		//CreateDXGIFactory1()��Factor1����邪�A���ۂ�Factor4�ȍ~�̃f�[�^���Ή����Ă���̂ŏ��Ƃ��Ă͓����Ă���
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
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
		desc.BufferCount = 2;
		//�o�͂���E�B���h�E�ւ̃E�B���h�E�n���h�����w��
		desc.OutputWindow = m_hWnd;
		//�o�b�N�o�b�t�@���̓���ւ���(Present�֐����Ă񂾎�)�̌���
		//D3D12�A�v���́ADXGI_SWAP_EFFECT_FLIP_SEQUENTIAL�܂���DXGI_SWAP_EFFECT_FLIP_DISCARD���g�p����K�v������
		//DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL: Present�֐����Ă񂾌�A�o�b�N�o�b�t�@�̓��e���ω����Ȃ�
		//DXGI_SWAP_EFFECT_FLIP_DISCARD: Present�֐����Ă񂾌�A�o�b�N�o�b�t�@�̓��e��j������
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		//�X���b�v�`�F�C������I�v�V����
		//DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH: ResizeTarget()���Ă�ŁA�\���T�C�Y��؂�ւ���
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGISwapChain* pSwapChain = nullptr;
		//IDXGISwapChain��IDXGISwapChain3�̐e�N���X
		//pSwapChain��IDXGISwapChain�̃N���X�����A
		//CreateSwapChain()�����Ƃ��A�Ή����Ă���q�N���X�̏���������Ă���
		hr = pFactory->CreateSwapChain(m_pQueue, &desc, &pSwapChain);
		if (FAILED(hr))
		{
			SafeRelease(pFactory);
			return false;
		}

		//CreateSwapChain()�����Ƃ��A�Ή����Ă���q�N���X�̏���������Ă���̂ŁA
		//���L��IDXGISwapChain3�̃|�C���^���擾�ł���
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
		if (FAILED(hr))
		{
			SafeRelease(pSwapChain);
			SafeRelease(pFactory);
			return false;
		}

		//�o�b�N�o�b�t�@�ԍ��̎擾. �`����index���擾�ł���
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
		SafeRelease(pSwapChain);
		SafeRelease(pFactory);

		//�R�}���h�A���P�[�^�[�̍쐬
		{
			for (auto i = 0u; i < FramCount; i++)
			{
				hr = m_pDevice->CreateCommandAllocator(
					//�R�}���h�A���P�[�^�[�̃^�C�v
					D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPU�����s�ł���R�}���h�o�b�t�@���w��
					IID_PPV_ARGS(&m_pCmdAllocateor[i])
				);
			}
		}

		//�R�}���h���X�g�̍쐬
		{
			hr = m_pDevice->CreateCommandList(
				0,		//GPU�m�[�h�̎���. 1����GPU�Ȃ��̂�0��ok
				//�R�}���h���X�g�̃^�C�v
				D3D12_COMMAND_LIST_TYPE_DIRECT,		//GPU�����s�ł���R�}���h�o�b�t�@���w��
				m_pCmdAllocateor[m_FrameIndex],		//�R�}���h�A���P�[�^�[�̎w��
				nullptr,		//�p�C�v���C���X�e�[�g�̎w��. ��Ŗ����I�ɐݒ肷��̂ł����ł�nullptr
				IID_PPV_ARGS(&m_pCmdList)
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
			desc.NumDescriptors = FramCount;		//�q�[�v���̃f�B�X�N���v�^��
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//�q�[�v�̃^�C�v���w��
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		//�q�[�v�̎g�p���@�A�Q�Ƃ̐����̌���
			desc.NodeMask = 0;		//GPU�m�[�h�̎w��

			//����
			hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeapRTV));
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

			for (auto i = 0u; i < FramCount; i++)
			{
				//�X���b�v�`�F�C�������o�b�t�@�̎擾. �����̓o�b�N�o�b�t�@�ԍ��Ɗi�[��
				hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pColorBuffer[i]));
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
				//�~�b�v�}�b�v���x���̔ԍ�(�X���b�v�`�F�C���쐬���Ƀ~�b�v�}�b�v���x����1�����Ȃ��悤�ɐݒ肵����0�Ԃ��w��)
				viewDesc.Texture2D.MipSlice = 0;
				//�g�p����e�N�X�`���̕��ʃX���C�X�ԍ�(���ʂ𕡐������f�[�^�ł͂Ȃ��̂�0���w��)
				viewDesc.Texture2D.MipSlice = 0;

				//�����_�[�^�[�Q�b�g�r���[����
				//���\�[�X: m_pColorBuffer, �ݒ�: viewDesc�̃����_�[�^�[�Q�b�g�r���[��handle�̈ʒuhandle�ɐ�������
				m_pDevice->CreateRenderTargetView(m_pColorBuffer[i], &viewDesc, handle);

				//CPU�f�B�X�N���v�^��handle��ۑ�
				m_HandleRTV[i] = handle;
				//����CPU�f�B�X�N���v�^�̃A�h���X�ֈړ�
				handle.ptr += incrementSize;
			}
		}
	}
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
	}
}

void App::TermApp()
{
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
