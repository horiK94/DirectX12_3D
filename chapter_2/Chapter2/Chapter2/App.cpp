#include "App.h"
namespace
{
	const auto ClassName = TEXT("SampleWindowClass");
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