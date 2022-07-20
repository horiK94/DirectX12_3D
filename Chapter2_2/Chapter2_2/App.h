#pragma once
#include <Windows.h>
#include <cstdint>

class App
{
public:
	//�����Ȃ�32bit�����^
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	//�C���X�^���X�n���h��
	HINSTANCE m_hInst;
	//�E�B���h�E�n���h��
	HWND m_hWnd;
	uint32_t m_Width;
	uint32_t m_Height;

	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

