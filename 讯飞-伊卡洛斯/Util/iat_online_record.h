#pragma once
#ifndef _iat_online_record_H
#define _iat_online_record_H

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <errno.h>
#include <process.h>

#include <msp_cmn.h>
#include <msp_errors.h>
#include "speech_recognizer.h"
enum {
	EVT_START = 0,
	EVT_STOP,
	EVT_QUIT,
	EVT_TOTAL
};
typedef void(_stdcall *AIAwakenMsgEnd)(void);
typedef void(_stdcall *AIAwakenMsgResult)(char *pString);
extern HANDLE events[EVT_TOTAL];

void AI_mic(const char* session_begin_params, AIAwakenMsgEnd pReocrd, AIAwakenMsgResult pResult);
void on_result(const char *result, char is_last);
void on_speech_begin();
void on_speech_end(int reason);
HANDLE start_helper_thread();
void show_key_hints(void);
void show_result(char *string, char is_over);


#endif // !_iat_online_record_H