#pragma once
#ifndef Process_H
#define Process_H
#include "Text.h"
#include <windows.h>
#include <TlHelp32.h>
#pragma warning(disable:4996)
class Mod_Process {
public:
	Mod_Process();
	~Mod_Process();
	//进程名取ID(进程名);
	DWORD ProcessNameTakeID(const char *ProcessName);
	//进程名取ID(进程名);
	DWORD ProcessNameTakeID(const wchar_t *ProcessName);
	//进程ID取窗口句柄(进程名);
	HWND ProcessIDTakeHWND(DWORD pid);
	//获取进程模块基址(进程ID,模块名);
	DWORD GetProcessModuleBaseAddress(DWORD pid, const char *ModuleName);
	//获取进程模块基址(进程ID,模块名);
	DWORD GetProcessModuleBaseAddress(DWORD pid, const wchar_t *ModuleName);
	//结束进程(进程名);
	BOOL TerminateProcess(const char *ProcessName);
	//结束进程(进程名);
	BOOL TerminateProcess(const wchar_t *ProcessName);
	//结束进程(进程ID);
	BOOL TerminateProcess(DWORD pid);
	//进程是否存在(进程名);
	BOOL ProcessIsExists(const char *ProcessName);
	//进程是否存在(进程名);
	BOOL ProcessIsExists(const wchar_t *ProcessName);
	//进程是否存在(进程ID);
	BOOL ProcessIsExists(DWORD pid);
	//运行进程(文件名,启动参数)
	void ProcessCreate(const char *pFileName, const char *pCmd);
	//运行进程(文件名,启动参数)
	void ProcessCreate(const wchar_t *pFileName, const wchar_t *pCmd);

private:

};
#endif

