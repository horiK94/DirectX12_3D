#include "main.h"
#include "App.h"

int wmain(int argc, wchar_t** envp)
{
	App app(960, 540);
	app.Run();
	return 0;
}