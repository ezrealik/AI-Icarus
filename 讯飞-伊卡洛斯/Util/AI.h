#pragma once

#ifndef _AI_H
#define _AI_H

#include <qisr.h>
#include <qivw.h>
#include <qtts.h>
#include <msp_cmn.h>
#include <msp_types.h>
#include <msp_errors.h>
#include "iat_online_record.h"
#include "AI_Shellcmd.h"
#include "AI_Awaken.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>

#pragma warning(disable:4996)
#pragma comment(lib,"winmm.lib")
#ifdef _WIN64
#pragma comment(lib,"msc_x64.lib")
#else
#pragma comment(lib,"msc.lib")
#endif // _WIN64


//装载语音识别系统
bool InstallAI();
//卸载语音识别系统
bool UninstallAI();

//AI唤醒处理线程
static void AIAwakenThread();
//AI语音识别处理线程
static void AIReordThread();
//AI录音函数
static void GetReord(void* pReordBuf, UINT *len);

//AI唤醒消息处理函数
static int AI_ivw_msg_proc(const char *sessionID, int msg, int param1, int param2, const void *info, void *userData);
//AI唤醒函数
static void AI_ivw_proc(void* pReordBuf, UINT Buflen, const char* session_begin_param);
//AI发声函数
static void AI_Say();

#endif // _AI_H