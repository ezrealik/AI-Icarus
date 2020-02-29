#pragma once
#ifndef _AI_text_to_speech_H
#define _AI_text_to_speech_H
#include <qivw.h>
#include <qtts.h>
#include <msp_cmn.h>
#include <msp_types.h>
#include <msp_errors.h>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>

#pragma warning(disable:4996)

/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int				size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int				fmt_size;				// = 下一个结构体的大小 : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = 通道数 : 1
	int				samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int				avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int       bits_per_sample;        // = 量化比特数: 8 | 16

	char            data[4];                // = "data";
	int				data_size;              // = 纯数据长度 : FileSize - 44 
} wave_pcm_hdr;

//取运行目录
void GetRunPath(char* RunPathBuf, int slen);
//文本转语音
int _stdcall text_to_speech(const char* src_text, const char* des_path, const char* params);
//播放AI语音
void _stdcall Play_AI_Speech(const char* src_text, const char* des_path, bool IsASYNC = false);
#endif