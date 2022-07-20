#pragma once
#include <Windows.h>
#include <cstdint>

class App
{
public:
	//符号なし32bit整数型
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	//インスタンスハンドル
	HINSTANCE m_hInst;
	//ウィンドウハンドル
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

