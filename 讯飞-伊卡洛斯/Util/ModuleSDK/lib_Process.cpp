#include "lib_Process.h"
namespace lib_Process
{
	Process::Process()
	{

	}

	Process::~Process()
	{

	}
	DWORD _fastcall Process::ProcessNameToPid(std::string ProcessName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hSnap == INVALID_HANDLE_VALUE)return 0;
		PROCESSENTRY32 pEntry32 = { sizeof(PROCESSENTRY32),0 };
		Process32First(hSnap, &pEntry32);
		DWORD pid;
		do 
		{
			if (strcmp(ProcessName.c_str(), pEntry32.szExeFile) == 0) {
				pid = pEntry32.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnap, &pEntry32));
		CloseHandle(hSnap);
		return pid;
	}
	DWORD _fastcall Process::ProcessNameToPid(std::wstring ProcessName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hSnap == INVALID_HANDLE_VALUE)return 0;
		PROCESSENTRY32W pEntry32 = { sizeof(PROCESSENTRY32W),0 };
		Process32FirstW(hSnap, &pEntry32);
		DWORD pid;
		do
		{
			if (wcscmp(ProcessName.c_str(), pEntry32.szExeFile) == 0) {
				pid = pEntry32.th32ProcessID;
				break;
			}
		} while (Process32NextW(hSnap, &pEntry32));
		CloseHandle(hSnap);
		return pid;
	}
	bool _fastcall Process::IsProcesNameExist(std::string ProcessName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hSnap == INVALID_HANDLE_VALUE)return 0;
		PROCESSENTRY32 pEntry32 = { sizeof(PROCESSENTRY32),0 };
		Process32First(hSnap, &pEntry32);
		bool IsExists = false;
		do
		{
			if (strcmp(ProcessName.c_str(), pEntry32.szExeFile) == 0) {
				IsExists = true;
				break;
			}
		} while (Process32Next(hSnap, &pEntry32));
		CloseHandle(hSnap);
		return IsExists;
	}
	bool _fastcall Process::IsProcesNameExist(std::wstring ProcessName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hSnap == INVALID_HANDLE_VALUE)return 0;
		PROCESSENTRY32W pEntry32 = { sizeof(PROCESSENTRY32W),0 };
		Process32FirstW(hSnap, &pEntry32);
		bool IsExists = false;
		do
		{
			if (wcscmp(ProcessName.c_str(), pEntry32.szExeFile) == 0) {
				IsExists = true;
				break;
			}
		} while (Process32NextW(hSnap, &pEntry32));
		CloseHandle(hSnap);
		return IsExists;
	}
	bool _fastcall Process::IsPidExist(DWORD pid)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hSnap == INVALID_HANDLE_VALUE)return 0;
		PROCESSENTRY32 pEntry32 = { sizeof(PROCESSENTRY32),0 };
		Process32First(hSnap, &pEntry32);
		bool IsExists = false;
		do
		{
			if (pEntry32.th32ProcessID == pid) {
				IsExists = true;
				break;
			}
		} while (Process32Next(hSnap, &pEntry32));
		CloseHandle(hSnap);
		return IsExists;
	}
	BOOL __stdcall EnumWindowsProc(HWND hWnd, LPARAM lParam)
	{
		DWORD pid = 0;
		GetWindowThreadProcessId(hWnd, &pid);
		if (pid == reinterpret_cast<DWORD*>(lParam)[0]) {
			reinterpret_cast<HWND*>(lParam)[1] = hWnd;
			return FALSE;
		}
		return TRUE;
	}
	HWND _fastcall Process::PidToHwnd(DWORD pid)
	{
		DWORD Info[2] = { 0 };
		Info[0] = pid;
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(Info));
		return reinterpret_cast<HWND>(Info[1]);
	}
	HWND _fastcall Process::ProcessNameToHwnd(std::string ProcessName)
	{
		DWORD pid = ProcessNameToPid(ProcessName);
		return PidToHwnd(pid);
	}
	HWND _fastcall Process::ProcessNameToHwnd(std::wstring ProcessName)
	{
		DWORD pid = ProcessNameToPid(ProcessName);
		return PidToHwnd(pid);
	}
	DWORD _fastcall Process::GetModuleAddress(std::string ProcessName, std::string ModuleName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessNameToPid(ProcessName));
		if (hSnap == INVALID_HANDLE_VALUE)return 0;
		MODULEENTRY32 pEntry32 = { sizeof(MODULEENTRY32),0 };
		Module32First(hSnap, &pEntry32);
		DWORD addr;
		do
		{
			if (strcmp(ModuleName.c_str(), pEntry32.szModule) == 0) {
				addr = reinterpret_cast<DWORD>(pEntry32.modBaseAddr);
				break;
			}
		} while (Module32Next(hSnap, &pEntry32));
		CloseHandle(hSnap);
		return addr;
	}
	DWORD _fastcall Process::GetModuleAddress(std::wstring ProcessName, std::wstring ModuleName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessNameToPid(ProcessName));
		if (hSnap == INVALID_HANDLE_VALUE)return 0;
		MODULEENTRY32W pEntry32 = { sizeof(MODULEENTRY32W),0 };
		Module32FirstW(hSnap, &pEntry32);
		DWORD addr;
		do
		{
			if (wcscmp(ModuleName.c_str(), pEntry32.szModule) == 0) {

				addr = reinterpret_cast<DWORD>(pEntry32.modBaseAddr);
				break;
			}
		} while (Module32NextW(hSnap, &pEntry32));
		CloseHandle(hSnap);
		return addr;
	}
	void _fastcall Process::ProcessRun(std::string FileName, std::string Parameter)
	{
		ShellExecuteA(NULL, "open", FileName.c_str(), Parameter.c_str(), NULL, SW_SHOWNORMAL);
	}
	void _fastcall Process::ProcessRun(std::wstring FileName, std::wstring Parameter)
	{
		ShellExecuteW(NULL, L"open", FileName.c_str(), Parameter.c_str(), NULL, SW_SHOWNORMAL);
	}

}

