#include "App.h"

int wmain(int argc, wchar_t** argv, wchar_t** envp)
{
	App app = App(960, 540);
	app.Run();
	return 0;
}