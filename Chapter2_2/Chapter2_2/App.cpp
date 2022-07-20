#include "App.h"

namespace
{
	const auto ClassName = TEXT("SampleWindowClass");		//�E�B���h�E�N���X��
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
	//�E�B���h�E������
	if (!InitWnd())
	{
		return false;
	}

	return true;
}

//term��terminator(�I��)�̗����Ǝv����
void App::TermApp()
{
	//�E�B���h�E�̏I������
	TermWnd();
}

//�E�B���h�E������
bool App::InitWnd()
{
	//�C���X�^���n���h���쐬
	auto hInst = GetModuleHandle(nullptr);
	{
		if (hInst == nullptr)
		{
			return false;
		}
	}

	//�E�B���h�E����
	{
		//�E�B���h�E�̐ݒ�
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);		//�\���̂̃T�C�Y
		wc.style = CS_HREDRAW | CS_VREDRAW;		//�E�B���h�E�X�^�C��
		//CS_HREDRAW: 	���T�C�Y���ς�����Ƃ��C�E�C���h�E�S�̂��ĕ`��
		//CS_VREDRAW: 	�c�T�C�Y���ς�����Ƃ��C�E�C���h�E�S�̂��ĕ`��
		wc.lpfnWndProc = WndProc;		//�� �E�B���h�E�̃��b�Z�[�W����������R�[���o�b�N�֐�
		wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);	//�A�C�R���n���h��
		wc.hCursor = LoadCursor(hInst, IDC_ARROW);		//�J�[�\���n���h��
		wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);		//�E�B���h�E�w�i
		wc.lpszMenuName = nullptr;		//���j���[��
		wc.lpszClassName = ClassName;		//�E�B���h�E�N���X�ɂ��閼�O
		wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION); //16*16�̏������A�C�R��

		//�E�C���h�E�N���X�o�^
		if (!RegisterClassEx(&wc))
		{
			return false;
		}

		m_hInst = hInst;

		//�E�B���h�E�̃T�C�Y�ݒ�
		RECT rc = { };
		rc.right = static_cast<LONG>(m_Width);
		rc.bottom = static_cast<LONG>(m_Height);

		//�E�B���h�E�X�^�C���ݒ�
		auto style = WS_OVERLAPPED		//	�^�C�g���o�[�C���E�����I�[�o�[���b�v�E�C���h�E
			| WS_CAPTION		//	�^�C�g���o�[(WS_DLGFRAME���ꏏ�ɂ��Ă���)
			| WS_SYSMENU;		// 	�^�C�g���o�[�ɃV�X�e�����j���[(����{�^���ƃA�C�R��)����
		//LPREC = RECT * �iWin32)
		// RECT rc; �� LPRECT lprc = &rc;
		AdjustWindowRect(&rc, style, FALSE);		//style�Ƒ�3��������rc�Ɍv�Z���ꂽ�l������ -> CreateWindow()�Ŏg�p

		//�E�B���h�E����
		m_hWnd = CreateWindowEx(		//CreateWindow�̊g���֐�
			0,		//�g���X�^�C��
			ClassName,		//�E�B���h�E�N���X�����w��
			TEXT("Sample"),		// �E�B���h�E�����w��B�^�C�g���o�[�Ȃǂɕ\�������
			style,		//�E�B���h�E�̃X�^�C��
			CW_USEDEFAULT,		//�E�B���h�E�̏����ʒu��x���W
			CW_USEDEFAULT,		//�E�B���h�E�̏����ʒu��y���W
			rc.right - rc.left,		//�E�B���h�E�̉���
			rc.bottom - rc.top,		//�E�B���h�E�̏c��
			nullptr,		//�쐬�����E�B���h�E�̐e�E�B���h�E or �I�[�i�[���w��
			nullptr,		//���j���[ or �q�E�B���h�EID���w��
			m_hInst,		//�E�B���h�E�Ɋ֘A�Â����C���X�^���X�n���h�����w��
			nullptr		//�A�v����CreateWindowEx �܂��� CreateWindow �֐����Ăяo���ăE�B���h�E�̍쐬��v�������Ƃ��ɁA�E�B���h�E�v���V�[�W���� WM_CREATE ���b�Z�[�W�� lParam �p�����[�^�Ƃ��ēn�����\���̃|�C���^
		);

		if (m_hWnd == nullptr)
		{
			return false;
		}

		//�E�B���h�E�̕\����Ԃ�ݒ�
		ShowWindow(m_hWnd, SW_SHOWNORMAL);		//SW_SHOWNORMAL: �E�B���h�E���A�N�e�B�u�ɂ��āA�\��. �ő剻 or �ŏ����̂Ƃ��͌��ɖ߂�

		//�E�B���h�E�̍ĕ`��v��(=WM_PAINT ���b�Z�[�W�𑗂�)
		UpdateWindow(m_hWnd);

		//�w�肵���E�B���h�E���t�H�[�J�X��Ԃɂ���
		SetFocus(m_hWnd);
	}

	return true;
}

void App::TermWnd()
{
	if (m_hInst != nullptr)
	{
		//�E�B���h�E�N���X�̓o�^����
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
		//�X���b�h�̃��b�Z�[�W�L���[�Ƀ��b�Z�[�W�����邩�ǂ������`�F�b�N
		if (PeekMessage(&msg,		//���b�Z�[�W�����i�[����|�C���^���w��
			nullptr,		//���b�Z�[�W���擾����E�B���h�E�̃n���h�����w�肷��B�S�Ă̏ꍇ�� NULL
			0,		//���b�Z�[�W�̍ŏ��l���w�肵�A�t�B���^�����O�B���Ȃ��ꍇ��0
			0,		//���b�Z�[�W�̍ő�l���w�肵�A�t�B���^�����O�B���Ȃ��ꍇ��0
			PM_REMOVE))		//���b�Z�[�W�̏������@. PM_REMOVE �Ȃ�΃��b�Z�[�W���L���[����폜
		{
			//���b�Z�[�W�擾����true
			TranslateMessage(&msg);		//�L�[���͂����ۂ̕����ɕϊ�����
			DispatchMessage(&msg);		//�E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W��n��
		}
	}
}

LRESULT App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:		//�E�B���h�E�j�����b�Z�[�W
	{
		//WM_QUIT���b�Z�[�W�L���[�𑗂�
		PostQuitMessage(0);
	}
		break;
	default:
		break;
	}
	
	//�E�B���h�E���b�Z�[�W�̋K�蓮����s��
	return DefWindowProc(hWnd, msg, wp, lp);
}
