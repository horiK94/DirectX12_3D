#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "main.h"
#include "App.h"

int wmain(int argc, wchar_t** envp)
{
#if defined(DEBUG) || defined(_DEBUG)
	//���������[�N�������������ɏo�̓E�B���h�E�ɏڍׂ��o�͂���f�o�R�}�t���O�𗧂Ă�
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	App app(960, 540);
	app.Run();
	return 0;
}