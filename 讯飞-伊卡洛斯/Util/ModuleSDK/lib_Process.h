#pragma once
#ifndef _LIB_PRCOESS_H
#define _LIB_PROCESS_H
#include <windows.h>
#include <iostream>
#include <sstream>
#include <TlHelp32.h>

namespace lib_Process
{
	class Process
	{
	public:
		Process();
		~Process();
		//PID获取窗口句柄
		//@parameter PID
		HWND _fastcall PidToHwnd(DWORD pid);
		//进程名获取窗口句柄
		//@parameter 进程名
		HWND _fastcall ProcessNameToHwnd(std::string ProcessName);
		//进程名获取窗口句柄
		//@parameter 进程名
		HWND _fastcall ProcessNameToHwnd(std::wstring ProcessName);
		//进程名获取PID
		//@parameter 进程名
		DWORD _fastcall ProcessNameToPid(std::string ProcessName);
		//进程名获取PID
		//@parameter 进程名
		DWORD _fastcall ProcessNameToPid(std::wstring ProcessName);
		//进程名是否存在
		//@parameter 进程名
		bool _fastcall IsProcesNameExist(std::string ProcessName);
		//进程名是否存在
		//@parameter 进程名
		bool _fastcall IsProcesNameExist(std::wstring ProcessName);
		//进程ID是否存在
		//@parameter 进程名
		bool _fastcall IsPidExist(DWORD pid);
		//获取模块地址
		//@parameter 进程名
		//@parameter 模块名
		DWORD _fastcall GetModuleAddress(std::string ProcessName, std::string ModuleName);
		//获取模块地址
		//@parameter 进程名
		//@parameter 模块名
		DWORD _fastcall GetModuleAddress(std::wstring ProcessName, std::wstring ModuleName);
		//获取模块地址
		//@parameter pid
		//@parameter 模块名
		DWORD _fastcall GetModuleAddress(DWORD pid, std::wstring ModuleName);
		//进程创建
		//@parameter 文件名
		//@parameter 运行参数
		//@parameter 返回结构体
		bool _fastcall ProcessCreate(std::string FileName, std::string Parameter, std::string ProcessStruct);
		//进程创建
		//@parameter 文件名
		//@parameter 运行参数
		//@parameter 返回结构体
		bool _fastcall ProcessCreate(std::wstring FileName, std::wstring Parameter, std::wstring ProcessStruct);
		//进程创建
		//@parameter 文件名
		//@parameter 运行参数
		void _fastcall ProcessRun(std::string FileName, std::string Parameter);
		//进程创建
		//@parameter 文件名
		//@parameter 运行参数
		void _fastcall ProcessRun(std::wstring FileName, std::wstring Parameter);
	private:

	};

}

#endif

