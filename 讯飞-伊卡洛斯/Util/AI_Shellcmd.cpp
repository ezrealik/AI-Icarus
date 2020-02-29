#include "AI_Shellcmd.h"

//是否关联上句话语
bool IsRelevance = false;
OpenAIShellCmd AIShellCmd = Cmd_None;
void GetRunPath(char* RunPathBuf, int slen)
{
	GetModuleFileNameA(GetModuleHandle(NULL), RunPathBuf, slen);
	char* pchar = strrchr(RunPathBuf, '\\');
	if (pchar)pchar[1] = 0;
}
void GetAutoLoginUserName(char *UserName,UINT slen)
{
	//获取Steam目录
	const char *SteamPathKey = "SOFTWARE\\Valve\\Steam";
	//HKEY_LOCAL_MACHINE\SOFTWARE\Classes\steam\Shell\Open\Command
	HKEY hKey;
	int Rkey = RegOpenKeyExA(HKEY_CURRENT_USER, SteamPathKey, NULL, KEY_WOW64_64KEY | KEY_READ, &hKey);
	char KeyData[1024] = { 0 }, *wp = nullptr;
	DWORD DataSize = 1024;
	int Rread = RegQueryValueExA(hKey, "AutoLoginUser", NULL, NULL, (BYTE*)KeyData, &DataSize);
	if (KeyData[0] != 0) {
		ZeroMemory(UserName, slen);
		CopyMemory(UserName, KeyData, strlen(KeyData));
	}

}
void SetAutoLoginUserName(const char *UserName)
{
	//获取Steam目录
	const char *SteamPathKey = "SOFTWARE\\Valve\\Steam";
	//HKEY_LOCAL_MACHINE\SOFTWARE\Classes\steam\Shell\Open\Command
	HKEY hKey;
	int Rkey = RegOpenKeyExA(HKEY_CURRENT_USER, SteamPathKey, NULL, KEY_WOW64_64KEY | KEY_WRITE, &hKey);
	char KeyData[1024] = { 0 };
	DWORD DataSize = 1024;
	int Rread = RegSetValueExA(hKey, "AutoLoginUser", NULL, REG_SZ, (BYTE*)UserName, strlen(UserName));

}
void GetSteamPath(char *SteamPath)
{
	//获取Steam目录
	const char *SteamPathKey = "SOFTWARE\\Classes\\steam\\Shell\\Open\\Command";
	//HKEY_LOCAL_MACHINE\SOFTWARE\Classes\steam\Shell\Open\Command
	HKEY hKey;
	int Rkey = RegOpenKeyExA(HKEY_LOCAL_MACHINE, SteamPathKey, NULL, KEY_WOW64_64KEY | KEY_READ, &hKey);
	char KeyData[1024] = { 0 }, *wp = nullptr;
	DWORD DataSize = 1024;
	int Rread = RegQueryValueExA(hKey, NULL, NULL, NULL, (BYTE*)KeyData, &DataSize);
	if (KeyData[0] == '\"') {
		wp = (char*)((char*)KeyData + (sizeof(char)));
		wp = strchr(wp, '\"');
		ZeroMemory(wp, 1);
		//使用steam启动游戏
		wp = (char*)((char*)KeyData + (sizeof(char)));
		CopyMemory(SteamPath, wp, (strlen(wp)*(sizeof(char))));
		strrchr(SteamPath, '\\')[1] = 0;
	}
}
void GetLoginUserInfo(std::vector<SteamUserInfo> &pSteamUserInfo,const char *SteamConfig)
{
	FILE *pFile = fopen(SteamConfig, "rb");
	if (!pFile) {
		const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
		const char* pSay_Text = "主人,伊卡洛斯获取Steam登录用户信息错误.";
		Play_AI_Speech(pSay_Text, filename, false);
		return;
	}
	fseek(pFile, 0, SEEK_END);
	int flen = ftell(pFile);
	void *pStruser = malloc(flen + 2);
	if (!pStruser) {
		const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
		const char* pSay_Text = "主人,伊卡洛斯申请内存失败.";
		Play_AI_Speech(pSay_Text, filename, false);
		return;
	}
	fseek(pFile, 0, SEEK_SET);
	fread(pStruser, flen, 1, pFile);
	char *ptemp = nullptr;
	ptemp = (char*)pStruser;
	wchar_t pUTF8[256] = { 0 };
	char pACP[256] = { 0 };
	while (ptemp)
	{
		Mod_Text Text;
		SteamUserInfo pTemp2;
		char *pUserName = Text.GetTextMiddle((char*)ptemp, "\"AccountName\"		\"", "\"");
		if (!pUserName)break;
		strcpy(pTemp2.pUserName, pUserName);
		char *pPersonaName = Text.GetTextMiddle((char*)ptemp, "\"PersonaName\"		\"", "\"");
		if (!pPersonaName)break;
		ZeroMemory(pACP, 256);
		ZeroMemory(pUTF8, 256);
		MultiByteToWideChar(CP_UTF8, NULL, pPersonaName, strlen(pPersonaName), pUTF8, 256);
		WideCharToMultiByte(CP_ACP, NULL, pUTF8, 256, pACP, 256, NULL, NULL);
		strcpy(pTemp2.pPersonaName, pACP);
		pSteamUserInfo.push_back(pTemp2);
		ptemp = strstr((char*)ptemp, "PersonaName");
		if (ptemp)ptemp += strlen("PersonaName");
	}
	fclose(pFile);
	free(pStruser);
}
void RunSteam()
{
	Mod_Process process;
	process.TerminateProcess("Steam.exe");
	process.TerminateProcess("Steam.exe");
	char SteamPath[1024] = { 0 };
	char SteamFile[1024] = { 0 };
	GetSteamPath(SteamPath);
	strcpy(SteamFile, SteamPath);
	strcat(SteamFile, "Steam.exe");
	ShellExecuteA(NULL, "open", "explorer.exe", SteamFile, SteamPath, SW_SHOWNORMAL);
}
void SwitchSteam(UINT nIndex)
{
	char SteamPath[1024] = { 0 };
	char SteamConfig[1024] = { 0 };
	std::vector<SteamUserInfo> pSteamUserInfo;
	GetSteamPath(SteamPath);
	strcpy(SteamConfig, SteamPath);
	strcat(SteamConfig, "config\\loginusers.vdf");
	GetLoginUserInfo(pSteamUserInfo, SteamConfig);
	if (pSteamUserInfo.empty()) {
		const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
		const char* pSay_Text = "主人,伊卡洛斯没有找到任何帐号信息,主人需要自己登录哦.";
		Play_AI_Speech(pSay_Text, filename, false);
		return;
	}
	if (nIndex == 0x6FF) {
		const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
		char pSay_Text[4096] = { 0 };
		wsprintfA(pSay_Text, "主人您一共有%d个帐号,您要切换哪一个账号呢?", pSteamUserInfo.size());
		for (UINT i = 0; i < pSteamUserInfo.size(); i++) {
			strcat(pSay_Text, "\n%d ");
			wsprintfA(pSay_Text, pSay_Text, i);
			strcat(pSay_Text, pSteamUserInfo[i].pPersonaName);
		}
		Play_AI_Speech(pSay_Text, filename, false);
		AIShellCmd = Steam_Cmd_Switch;
		SetEvent(events[EVT_START]);
	}
	else {
		const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
		char pSay_Text[4096] = { 0 };
		if (nIndex < pSteamUserInfo.size()) {
			wsprintfA(pSay_Text, "主人,伊卡洛斯正在为您登录 %s 请耐心等待", pSteamUserInfo[nIndex].pPersonaName);
			Play_AI_Speech(pSay_Text, filename, false);
			SetAutoLoginUserName(pSteamUserInfo[nIndex].pUserName);
			RunSteam();
		}
		else {
			wsprintfA(pSay_Text, "主人,伊卡洛斯无法登录该账户,原因是传递的索引太大啦 索引值为 %d.", nIndex);
			Play_AI_Speech(pSay_Text, filename, false);
		}
	}
}
void _stdcall OpenSteam()
{
	char SteamPath[1024] = { 0 };
	char SteamFile[1024] = { 0 };
	char UserName[1024];
	GetSteamPath(SteamPath);
	//判断Steam是否已经运行
	lib_Process::Process process;
	if (process.IsProcesNameExist("Steam.exe")) {
		const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
		const char* pSay_Text = "主人,Steam已经在运行了,您需要切换Steam账号吗?";
		Play_AI_Speech(pSay_Text, filename, false);
		AIShellCmd = Steam_Cmd_Isexst;
		SetEvent(events[EVT_START]);
		return;
	}
	const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
	char pSay_Text[256];
	GetAutoLoginUserName(UserName, 1024);
	wsprintfA(pSay_Text, "主人,您现在登录的账号是 %s ,需要为主人切换账号吗?", UserName);
	Play_AI_Speech(pSay_Text, filename, false);
	AIShellCmd = Steam_Cmd_Open;
	SetEvent(events[EVT_START]);
	return;
}
void ShutDownComputer()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES Token_Privileges;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	LookupPrivilegeValueA(NULL, "SeShutdownPrivilege", &Token_Privileges.Privileges->Luid);
	Token_Privileges.PrivilegeCount = 1;
	Token_Privileges.Privileges->Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &Token_Privileges, NULL, NULL, NULL);
	int ret = ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION);
	if (!ret) {
		ret = GetLastError();
		if (ret = 1314) {
			const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
			const char *pSay_Text = "主人,电脑关闭失败,原因是伊卡洛斯没有权限.";
			Play_AI_Speech(pSay_Text, filename, false);
		}
		else {
			const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
			char pSay_text2[500];
			wsprintfA(pSay_text2, "主人,电脑关闭失败 错误代码是 %d .", ret);
			Play_AI_Speech(pSay_text2, filename, false);
		}
	}
}
void _stdcall AI_ShellCmd(char *pString)
{
	const char* filename = "audio\\tts_Open.wav"; //合成的语音文件名称
	if (strstr(pString, "打开steam") || strstr(pString, "玩steam") ||
		strstr(pString, "打开视频")//因为发音原因有时候会识别成视频.....
		) {
		const char *pSay_Text = "好的主人,正在为您打开steam,要耐心等待哦.";
		Play_AI_Speech(pSay_Text, filename, false);
		OpenSteam();
	}
	else if (strstr(pString, "时间") || strstr(pString, "几点")) {
		SYSTEMTIME ptime;
		GetLocalTime(&ptime);
		char pSay_text2[500];
		wsprintfA(pSay_text2, "现在时间是 %d年%d月%d日 %d点%d分%d秒.", ptime.wYear, ptime.wMonth, ptime.wDay, ptime.wHour, ptime.wMinute, ptime.wSecond);
		Play_AI_Speech(pSay_text2, filename, false);
	}
	else if (strstr(pString, "不是叫你") || strstr(pString, "没有喊你") || strstr(pString, "不是说你") || strstr(pString, "没有说你")) {
		const char *pSay_Text = "好吧,有什么需要尽管吩咐伊卡洛斯.";
		Play_AI_Speech(pSay_Text, filename, false);
	}
	else if (strstr(pString, "电脑关机") || strstr(pString, "关掉电脑") || strstr(pString, "关闭电脑") || strstr(pString, "电脑关掉") || strstr(pString, "关电脑")) {
		const char *pSay_Text = "好的,正在关闭计算机.";
		Play_AI_Speech(pSay_Text, filename, false);
		ShutDownComputer();
	}
	else {
		int Num = 0;
		switch (AIShellCmd)
		{
		case Answer_Cmd_No_ops:
			AIShellCmd = Cmd_None;
			if (strstr(pString, "不用了") || strstr(pString, "没有了") || strstr(pString, "不需要你")) {
				const char *pSay_Text = "好吧,有什么需要尽管吩咐伊卡洛斯.";
				Play_AI_Speech(pSay_Text, filename, false);
			}
			else {
				goto Else_Exitr;
			}
			return;
		case Steam_Cmd_Isexst://如果Steam已经启动,则询问是否切换账号
			AIShellCmd = Cmd_None;
			if (strstr(pString, "不用了") || strstr(pString, "没有了") || strstr(pString, "不需要") || strstr(pString, "不切换")) {
				const char *pSay_Text = "好的主人,正在为您重新打开Steam,请耐心等待.";
				Play_AI_Speech(pSay_Text, filename, false);
				RunSteam();
			}
			else if(strstr(pString, "要的") || strstr(pString, "需要") || strstr(pString, "换账号") || strstr(pString, "确定"))
			{
				const char *pSay_Text = "好的主人,您需要切换什么帐号呢?.";
				Play_AI_Speech(pSay_Text, filename, false);
				SwitchSteam(0x6FF);
			}
			else {
				goto Else_Exitr;
			}
			return;
		case Steam_Cmd_Open://如果Steam没有启动,同样询问是否切换账号
			AIShellCmd = Cmd_None;
			if (strstr(pString, "不用了") || strstr(pString, "没有了") || strstr(pString, "不需要") || strstr(pString, "不切换")) {
				const char *pSay_Text = "好的主人,正在为您重新打开Steam,请耐心等待.";
				Play_AI_Speech(pSay_Text, filename, false);
				RunSteam();
			}
			else if (strstr(pString, "要的") || strstr(pString, "需要") || strstr(pString, "换账号") || strstr(pString, "确定"))
			{
				const char *pSay_Text = "好的主人,正在为您获取Steam帐号信息.";
				Play_AI_Speech(pSay_Text, filename, false);
				SwitchSteam(0x6FF);
			}
			else {
				goto Else_Exitr;
			}
			return;
		case Steam_Cmd_Switch://如果Steam没有启动,同样询问是否切换账号
			AIShellCmd = Cmd_None;
			if (pString[0] <= '9'&&pString[0] >= '0') {
				Num = atoi(pString);
				SwitchSteam(Num);
			}
			else {
				const char *pSay_Text_ = "请主人选择帐号正确的数字.";
				Play_AI_Speech(pSay_Text_, filename, false);
				AIShellCmd = Steam_Cmd_Switch;
				SetEvent(events[EVT_START]);
			}
			return;
		default:
			break;
		}
		char pSay_Text[256];
		wsprintfA(pSay_Text, "主人您说的 %s ,伊卡洛斯无法做到,还需要伊卡洛斯做其他事情吗?.",pString);
		Play_AI_Speech(pSay_Text, filename, false);
		AIShellCmd = Answer_Cmd_No_ops;
		SetEvent(events[EVT_START]);
		return;
	Else_Exitr:
		const char *pSay_Text_ = "主人在说什么,伊卡洛斯无法理解主人的意图.";
		Play_AI_Speech(pSay_Text_, filename, false);
	}

}