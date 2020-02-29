#include "lib_Hook.h"
namespace lib_Hook
{
	static std::vector<HookInfo> pHookArray;
	Hook::Hook()
	{
		
	}

	Hook::~Hook()
	{
	}
	//地址取整;
	inline void* Round(void *addr, size_t rndown)
	{
		size_t ior = (size_t)addr / rndown;
		return(void*)(((size_t)addr / rndown)*rndown);
	}
	void* _fastcall Hook::LocalAlloc(void *DestAddres, size_t size)
	{
		PBYTE pLower = (PBYTE)DestAddres;
		pLower = pLower < (PBYTE)(DWORD_PTR)0x0000000080000000 ? (PBYTE)(0x1) : (PBYTE)(pLower - (PBYTE)0x7fff0000);
		PBYTE pUpper = (PBYTE)DestAddres + 4096;
		pUpper = pUpper < (PBYTE)(DWORD_PTR)0xffffffff80000000 ? (PBYTE)(pUpper + (DWORD_PTR)0x7ff80000) : (PBYTE)(DWORD_PTR)0xfffffffffff80000;
		SYSTEM_INFO sSysInfo = { 0 };
		::GetSystemInfo(&sSysInfo);
		//64KB大小分配,因为VirtualAlloc申请内存是以64KB粒度范围分配.
		size_t cAllocSize = max(sSysInfo.dwAllocationGranularity, 4096);
		cAllocSize = size < cAllocSize ? cAllocSize : size;
		PBYTE pRetVal = nullptr;
		PBYTE pModuleGuess = (PBYTE)(Round(DestAddres, cAllocSize));//(PBYTE)DestAddres; //
		int loopCount = 0;
		for (PBYTE pbAlloc = pModuleGuess; pLower < pbAlloc && pbAlloc < pUpper; ++loopCount) {
			//查询地址是否是未分配状态
			MEMORY_BASIC_INFORMATION mbi;
			if (!VirtualQuery(pbAlloc, &mbi, sizeof(mbi)))
				break;
			//判断地址是否未分配及大小是否足够.
			if (mbi.State == MEM_FREE && mbi.RegionSize >= (unsigned)cAllocSize) {
				//分配内存空间,64KB粒度分配;因为VirtualAlloc申请内存是以64KB粒度范围分配;
				pRetVal = (PBYTE)VirtualAlloc(pbAlloc, cAllocSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				break;
			}
			//螺旋扫描未分配的内存地址, -1, 1, -2, 2, -3, 3,(* cAllocSize)
			size_t bytesToOffset = (cAllocSize * (loopCount + 1) * ((loopCount % 2 == 0) ? -1 : 1));
			pbAlloc = pbAlloc + bytesToOffset;
		}
		return pRetVal;
	}
	void _fastcall Hook::LocalFree(void* lpbuff)
	{
		VirtualFree(lpbuff, 0, MEM_RELEASE);
	}
	//Detour修改Jmp跳转 x86_x64兼容;
	inline PBYTE detour_gen_jmp(PBYTE pbCode, PBYTE pbJumpTo)
	{
		PBYTE pbJumpFrom = pbCode + 5;
		SIZE_T cbDiff = pbJumpFrom > pbJumpTo ? pbJumpFrom - pbJumpTo : pbJumpTo - pbJumpFrom;
		if (cbDiff <= 0x7fff0000) {
			*pbCode++ = 0xe9;
			*((PDWORD)pbCode) = (DWORD)(DWORD_PTR)(pbJumpTo - pbJumpFrom);
			pbCode += sizeof(DWORD);
		}
		else {
			*pbCode++ = 0xff;
			*pbCode++ = 0x25;
			// on x64 we write the relative address of the same location
			*((PDWORD)pbCode) = (DWORD)0;
			pbCode += sizeof(DWORD);
			*((PDWORD_PTR)pbCode) = (DWORD_PTR)(pbJumpTo);
			pbCode += sizeof(DWORD_PTR);

		}
		return pbCode;

	}
	void* _fastcall Hook::SetHook(void* DestAddress, size_t length, void* NewFunction)
	{
		DWORD OldProtect;
		HookInfo Tmp;
		if (!VirtualProtect(DestAddress, length, PAGE_EXECUTE_READWRITE, &OldProtect))return nullptr;
#ifdef _WIN64
		//申请中继跳转;
		void *pRelayJmp = LocalAlloc(DestAddress, length + 256);
		if (!pRelayJmp)return nullptr;
		Tmp.pAlloc = pRelayJmp;
		//复制原函数代码;
		CopyMemory((LPVOID)((char*)pRelayJmp + 14), DestAddress, length);
		//HOOK 跳转;
		*((WORD*)pRelayJmp) = MAKEWORD(0xFF, 0x25);
		*((__int64*)((char*)pRelayJmp + 6)) = (__int64)NewFunction;
		//跳转到源函数;
		pRelayJmp = (char*)pRelayJmp + (length + 14);
		((char*)pRelayJmp)[0] = (char)0xE9;
		*((DWORD*)((char*)pRelayJmp + 1)) = (DWORD)DestAddress - (DWORD)((char*)pRelayJmp - (length)) - 5;
		memset(DestAddress, 0x90, length);
		((char*)DestAddress)[0] = (char)0xE9;
		*((DWORD*)((char*)DestAddress + 1)) = (DWORD)((char*)pRelayJmp - (length + 14)) - (DWORD)DestAddress - 5;
		//保存Hook信息;
		pRelayJmp = (char*)pRelayJmp - length;
		Tmp.HookLength = length;
		Tmp.pNewFun = NewFunction;
		Tmp.pHookAddress = DestAddress;
		Tmp.pOldFunCall = pRelayJmp;
		pHookArray.push_back(Tmp);
		VirtualProtect(DestAddress, length, OldProtect, &OldProtect);
		return pRelayJmp;
#else
		//申请中继跳转;
		void *pRelayJmp = VirtualAlloc(NULL, length + 256, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!pRelayJmp)return nullptr;
		Tmp.pAlloc = pRelayJmp;
		//复制原函数代码;
		CopyMemory((LPVOID)((char*)pRelayJmp + 5), DestAddress, length);
		//HOOK 跳转;
		((char*)pRelayJmp)[0] = (char)0xE9;
		*((DWORD*)((char*)pRelayJmp + 1)) = (DWORD)NewFunction - (DWORD)pRelayJmp - 5;
		//跳转到源函数;
		pRelayJmp = (char*)pRelayJmp + (length + 5);
		((char*)pRelayJmp)[0] = (char)0xE9;
		*((DWORD*)((char*)pRelayJmp + 1)) = (DWORD)DestAddress - (DWORD)((char*)pRelayJmp - length) - 5;
		memset(DestAddress, 0x90, length);
		((char*)DestAddress)[0] = (char)0xE9;
		*((DWORD*)((char*)DestAddress + 1)) = (DWORD)((char*)pRelayJmp - (length + 5)) - (DWORD)DestAddress - 5;
		//保存Hook信息;
		pRelayJmp = (char*)pRelayJmp - length;
		Tmp.HookLength = length;
		Tmp.pNewFun = NewFunction;
		Tmp.pHookAddress = DestAddress;
		Tmp.pOldFunCall = pRelayJmp;
		pHookArray.push_back(Tmp);
		VirtualProtect(DestAddress, length, OldProtect, &OldProtect);
		return pRelayJmp;
#endif // _WIN64
	}
	HookInfo _fastcall Hook::QueryHookInfo(void* DestAddress, void* NewFunction)
	{
		HookInfo tmp = { 0 };
		for (size_t i = 0; i < pHookArray.size(); i++) {
			if (pHookArray[i].pHookAddress == DestAddress && pHookArray[i].pNewFun == NewFunction) {
				tmp = pHookArray[i];
				break;
			}
		}
		return tmp;
	}
	bool _fastcall Hook::SetHookState(void* DestAddress, void* NewFunction, bool IsPause)
	{
		bool IsPauses = false;
		for (unsigned int i = 0; i < pHookArray.size(); i++) {
			if (pHookArray[i].pHookAddress == DestAddress && pHookArray[i].pNewFun == NewFunction) {
				if (IsPause) {
					DWORD OldProtect;
					if (!VirtualProtect(pHookArray[i].pHookAddress, pHookArray[i].HookLength, PAGE_EXECUTE_READWRITE, &OldProtect))break;
					CopyMemory(pHookArray[i].pHookAddress, pHookArray[i].pOldFunCall, pHookArray[i].HookLength);
					if (!VirtualProtect(pHookArray[i].pHookAddress, pHookArray[i].HookLength, OldProtect, &OldProtect))break;
				}
				else {
					DWORD OldProtect;
					if (!VirtualProtect(pHookArray[i].pHookAddress, pHookArray[i].HookLength, PAGE_EXECUTE_READWRITE, &OldProtect))break;
					((char*)DestAddress)[0] = (char)0xE9;
					*((DWORD*)((char*)DestAddress + 1)) = (DWORD)pHookArray[i].pAlloc - (DWORD)DestAddress - 5;
					if (!VirtualProtect(pHookArray[i].pHookAddress, pHookArray[i].HookLength, OldProtect, &OldProtect))break;
				}
			}
		}
		return IsPauses;
	}
	bool _fastcall Hook::UnHook(void* DestAddress, void* NewFunction)
	{
		bool IsUninstall = false;
		for (size_t i = 0; i < pHookArray.size(); i++) {
			if (pHookArray[i].pHookAddress == DestAddress && pHookArray[i].pNewFun == NewFunction) {
				DWORD OldPrcotect = 0;
				VirtualProtect(pHookArray[i].pHookAddress, pHookArray[i].HookLength, PAGE_EXECUTE_READWRITE, &OldPrcotect);
				CopyMemory(pHookArray[i].pHookAddress, pHookArray[i].pOldFunCall, pHookArray[i].HookLength);
				VirtualProtect(pHookArray[i].pHookAddress, pHookArray[i].HookLength, OldPrcotect, &OldPrcotect);
				LocalFree(pHookArray[i].pAlloc);
				IsUninstall = true;
				break;
			}
		}
		return IsUninstall;
	}
	void _fastcall Hook::ReleaseHook()
	{
		for (unsigned int i = 0; i < pHookArray.size(); i++) {
			UnHook(pHookArray[i].pHookAddress, pHookArray[i].pNewFun);
		}
		//删除所有Hook信息数据;
		pHookArray.clear();
		//释放所有Hook信息占用的内存;
		std::vector<HookInfo>().swap(pHookArray);
	}
}