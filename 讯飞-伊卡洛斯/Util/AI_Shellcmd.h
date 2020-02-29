#pragma once
#ifndef _AI_Shellcmd_H
#define _AI_Shellcmd_H

#include "AI_text_to_speech.h"
#include "iat_online_record.h"
#include "json/json.h"
#include "ModuleSDK/Module.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#pragma warning(disable:4996)

enum OpenAIShellCmd
{
	Cmd_None,//无任何下一步指令
	Steam_Cmd_Open,//打开Steam
	Steam_Cmd_Isexst,//Steam是否存在
	Steam_Cmd_Switch,//切换账号
	Answer_Cmd_No_ops,//没有任何操作的应答
};
struct SteamUserInfo
{
	char pUserName[256];
	char pPersonaName[256];
};
void _stdcall OpenSteam();
void _stdcall AI_ShellCmd(char *pString);

#endif
