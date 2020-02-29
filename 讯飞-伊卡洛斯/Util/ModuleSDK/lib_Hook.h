#pragma once
#ifndef _LIB_HOOK_H
#define _LIB_HOOK_H
#include <windows.h>
#include <tchar.h>
#include <vector>
//x86-x64兼容
namespace lib_Hook
{
	struct HookInfo
	{
		void *pHookAddress = nullptr;
		size_t HookLength = 0;
		void *pNewFun = nullptr;
		void *pOldFunCall = nullptr;
		void *pAlloc = nullptr;
	};
	class Hook
	{
	public:
		Hook();
		~Hook();
		//设置Hook
		//@parameter 目标地址
		//@parameter 地址要保存地址长度
		//@parameter 新的函数地址
		void* _fastcall SetHook(void* DestAddress, size_t length, void* NewFunction);
		//卸载Hook
		//@parameter 目标地址
		//@parameter 新的函数地址
		bool _fastcall UnHook(void* DestAddress, void* NewFunction);
		//暂停Hook
		//@parameter 目标地址
		//@parameter 是否暂停Hook
		bool _fastcall SetHookState(void* DestAddress, void* NewFunction, bool IsPause);
		//查询Hook信息
		//@parameter 目标地址
		//@parameter 新的函数地址
		HookInfo _fastcall QueryHookInfo(void* DestAddress, void* NewFunction);
		//在目标地址附近申请内存
		//@parameter 目标地址
		//@parameter 大小
		void* _fastcall LocalAlloc(void* DestAddres, size_t size);
		//释放申请的内存
		//@parameter 目标地址
		//@parameter 已申请的内存大小
		void _fastcall LocalFree(void* lpbuff);
		//释放所有HOOK;
		void _fastcall ReleaseHook();
	private:
		
	};
}

#endif // !_LIB_HOOK_H
