#include "main.h"

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	_tprintf(_T("伊卡洛斯智能模块正在加载中....\n"));
	
	_tprintf(_T("*.开始装载AI系统..\n"));
	if (!InstallAI()) {
		_tprintf(_T("*.AI系统装载失败!\n"));
	}
	_tprintf(_T("*.AI系统成功装载!\n"));
	
	while (true)
	{
		Sleep(20);
	}
	_tprintf(_T("*.正在卸载AI系统..\n"));
	if (!UninstallAI()) {
		_tprintf(_T("*.AI系统卸载失败!\n"));
	}
	system("pause");
	return 0;
}