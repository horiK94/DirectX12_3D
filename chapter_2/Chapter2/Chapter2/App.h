#pragma once
// pragma onceはshaderの#defineのもうちょっと速い番みたいなもの。こっちのほうが速く、キーワードが使われないので推奨される

#include <Windows.h>
#include <cstdint>

class App
{
public:
	App(uint32_t width, uint32_t height);
	~App();
	void Run();

private:
	HINSTANCE m_hInst;		//インスタンスハンドル
	HWND m_hWnd;			//ウィンドウハンドル
	uint32_t m_Width;		//ウィンドウの横幅
	uint32_t m_Height;		//ウィンドウの縦幅

	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

