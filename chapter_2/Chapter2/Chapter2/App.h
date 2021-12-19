#pragma once
// pragma once��shader��#define�̂���������Ƒ����Ԃ݂����Ȃ��́B�������̂ق��������A�L�[���[�h���g���Ȃ��̂Ő��������

#include <Windows.h>
#include <cstdint>

class App
{
public:
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	HINSTANCE m_hInst;		//�C���X�^���X�n���h��
	HWND m_hWnd;			//�E�B���h�E�n���h��
	uint32_t m_Width;		//�E�B���h�E�̉���
	uint32_t m_Height;		//�E�B���h�E�̏c��

	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

