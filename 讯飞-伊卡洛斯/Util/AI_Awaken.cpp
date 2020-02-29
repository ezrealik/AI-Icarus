#include "AI_Awaken.h"

//打开录音设备
static bool __stdcall recorder_open_device(HWAVEIN *hWaveIN, WAVEFORMATEX *hFormat, HANDLE hThread);
//创建录音缓冲区
static bool __stdcall recorder_Add_buffer(recorder_info *rec);

void __stdcall recorder_thread(recorder_info *rec)
{
	int bRet;
	MSG uMsg;
	PeekMessage(&uMsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	while ((bRet = GetMessage(&uMsg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			continue;
		}
		switch (uMsg.message)
		{
		case MM_WIM_OPEN:
			//if (rec->call.on_begin_bind)rec->call.on_begin_bind();
			waveInStop(rec->hWaveIN);
			printf("MM_MIM_OPEN...\n");
			break;
		case MM_WIM_CLOSE:
			//if (rec->call.on_begin_bind)rec->call.on_close_bind();
			printf("MM_MIM_CLOSE...\n");
			break;
		case MM_WIM_DATA:
			//if (rec->call.on_begin_bind)rec->call.on_data_bind(rec->bufheader, rec->buflen);
			printf("MM_MIM_DATA...\n");
			break;
		default:
			printf("message.%X \n", uMsg.message);
			break;
		}
		Sleep(1);
	}
}
bool __stdcall recorder_start(AwakenCallBack AwakenCallBack2, const char *session_begin_params)
{
	HWAVEIN hWaveIN;
	HANDLE hThread;
	recorder_info rec;
	int ret;
	if (!AwakenCallBack2)return false;
	rec.call.callback_bind = AwakenCallBack2;
	hThread = (HANDLE)_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)recorder_thread, &rec, NULL, NULL);
	if (!recorder_open_device(&hWaveIN, NULL, hThread))return false;
	rec.rec_thread_hdl = hThread;
	rec.hWaveIN = hWaveIN;
	rec.buflen = 1024;
	rec.bufheader = malloc(rec.buflen);
	if (!rec.bufheader) {
		waveInReset(hWaveIN);
		return false;
	}
	if (!recorder_Add_buffer(&rec)) {
		waveInReset(hWaveIN);
		return false;
	}
	ret = waveInStart(hWaveIN);
	if (ret != MMSYSERR_NOERROR) {
		waveInStop(hWaveIN);
		waveInReset(hWaveIN);
		return false;
	}
	while (true)
	{
		Sleep(1);
	}
	return true;
}
bool __stdcall recorder_Add_buffer(recorder_info *rec)
{
	int ret;
	rec->wavein_hdl.dwBufferLength = rec->buflen;
	rec->wavein_hdl.dwFlags = WAVE_FORMAT_PCM;
	rec->wavein_hdl.dwBytesRecorded = 0;
	rec->wavein_hdl.dwLoops = 1;
	rec->wavein_hdl.dwUser = 0;
	rec->wavein_hdl.lpNext = 0;
	rec->wavein_hdl.reserved = 0;
	rec->wavein_hdl.lpData = (LPSTR)rec->bufheader;
	ret = waveInPrepareHeader(rec->hWaveIN, &rec->wavein_hdl, sizeof(WAVEHDR));
	if (ret != MMSYSERR_NOERROR)return false;
	ret = waveInAddBuffer(rec->hWaveIN, &rec->wavein_hdl, sizeof(WAVEHDR));
	if (ret != MMSYSERR_NOERROR)return false;
	return true;
}
bool __stdcall recorder_open_device(HWAVEIN *hWaveIN, WAVEFORMATEX *hFormat, HANDLE hThread)
{
	WAVEFORMATEX *pFormat = hFormat, Tmp_Format;
	int ret = 0;
	Tmp_Format.cbSize = sizeof(WAVEFORMATEX);
	Tmp_Format.nChannels = 1;
	Tmp_Format.nBlockAlign = 2;
	Tmp_Format.wFormatTag = WAVE_FORMAT_PCM;
	Tmp_Format.wBitsPerSample = 16;
	Tmp_Format.nSamplesPerSec = 16000;
	Tmp_Format.nAvgBytesPerSec = 16000;
	if (!pFormat)pFormat = &Tmp_Format;
	ret = waveInOpen(hWaveIN, WAVE_MAPPER, pFormat, GetThreadId(hThread), (DWORD_PTR)0, CALLBACK_THREAD);
	if (ret != MMSYSERR_NOERROR | hWaveIN == nullptr)return false;
	return true;
}
