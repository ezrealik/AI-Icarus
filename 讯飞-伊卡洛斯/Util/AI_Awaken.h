
#pragma once
#ifndef _AI_AWAKEN_H
#define _AI_AWAKEN_H


#include <qivw.h>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

typedef struct recorder_call {
	void(__stdcall *callback_bind)(struct recorder_info *rec);
	void(__stdcall *on_begin_bind)();
	void(__stdcall *on_data_bind)(void *data, UINT len);
	void(__stdcall *on_close_bind)();
};
typedef struct recorder_speech {
	const char *session_id;
	int ep_stat;
	int rec_stat;
	int audio_status;
	const char *session_begin_params;
};
typedef struct recorder_info {
	recorder_call call;
	recorder_speech speech;
	HWAVEIN hWaveIN;
	void *user_cb_para;
	WAVEHDR wavein_hdl;
	void *rec_thread_hdl;
	void *bufheader;
	UINT buflen;
	volatile int state;
};
typedef void(__stdcall *AwakenCallBack)(struct recorder_info *rec);
//³õÊ¼»¯Â¼Òô
bool __stdcall recorder_start(AwakenCallBack AwakenCallBack2, const char *session_begin_params);

#endif

