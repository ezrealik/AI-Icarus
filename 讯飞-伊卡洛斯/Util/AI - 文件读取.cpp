#include "AI.h"

#define IVW_AUDIO_FILE_NAME "audio\\awake2.pcm"
#define FRAME_LEN	640 //16k采样率的16bit音频，一帧的大小为640B, 时长20ms

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
/* 默认wav音频头部数据 */
wave_pcm_hdr default_wav_hdr =
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0
};
//说话文本
char *pSay_Text = nullptr;
//是否唤醒
bool IsAwaken = false;
bool InstallAI()
{
	const char* login_params = "appid = 5e468965,work_dir = .";
	if (MSPLogin(NULL, NULL, login_params) != MSP_SUCCESS)return false;
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AIAwakenThread, NULL, NULL, NULL);
	return true;
}
bool UninstallAI()
{
	if (MSPLogout() != MSP_SUCCESS)return false;
	return true;
}

void AIAwakenThread()
{
	const char *ssb_param = "ivw_threshold=0:1050,sst=wakeup,ivw_res_path =fo|res/ivw/wakeupresource.jet";
	char Path[256];
	while (true)
	{
		if (!IsAwaken) {
			int buflen = 1024 * 100;
			void* pReordBuf = malloc(buflen);
			if (!pReordBuf) {
				_tprintf(_T("*.内存申请失败,请确认内存是否充足!\n"));
				break;
			}
			ZeroMemory(pReordBuf, buflen);
			GetReord(pReordBuf, buflen);
			/*GetRunPath(Path, sizeof(Path));
			strcat(Path, IVW_AUDIO_FILE_NAME);
			FILE *pFile = fopen(Path, "wb");
			if (!pFile) {
				_tprintf(_T("*.[audio\\awake.pcm]文件操作失败!\n"));
				continue;
			}
			fwrite(pReordBuf, 1, buflen, pFile);
			fclose(pFile);*/
			AI_ivw_proc(NULL, Path, ssb_param, pReordBuf, buflen);
			free(pReordBuf);
		}
		Sleep(1);
	}
}
void AIReordThread()
{
	const char* session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";
	char Path[256];
	while (true)
	{
		if (IsAwaken) {
			int buflen = 1024 * 100;
			void* pReordBuf = malloc(buflen);
			if (!pReordBuf) {
				_tprintf(_T("*.内存申请失败,请确认内存是否充足!\n"));
				break;
			}
			ZeroMemory(pReordBuf, buflen);
			GetReord(pReordBuf, buflen);
			GetRunPath(Path, sizeof(Path));
			strcat(Path, IVW_AUDIO_FILE_NAME);
			FILE *pFile = fopen(Path, "wb");
			if (!pFile) {
				_tprintf(_T("*.[audio\\awake.pcm]文件操作失败!\n"));
				continue;
			}
			fwrite(pReordBuf, 1, buflen, pFile);
			fclose(pFile);
			
			free(pReordBuf);
		}
		Sleep(1);
	}
}
void GetReord(void* pReordBuf, int len)
{
	if (len <= 0)return;
	if (!pReordBuf)return;
	HWAVEIN hWaveIn;
	WAVEFORMATEX WaveForm;
	WAVEHDR WaveHdr;
	WaveForm.nBlockAlign = 2;
	WaveForm.nSamplesPerSec = 16000;
	WaveForm.wBitsPerSample = 16;
	WaveForm.nAvgBytesPerSec = 16000;
	WaveForm.nChannels = 1;
	WaveForm.wFormatTag = WAVE_FORMAT_PCM;
	WaveForm.cbSize = sizeof(WAVEFORMATEX);
	HANDLE wait = CreateEvent(NULL, NULL, NULL, NULL);
	waveInOpen(&hWaveIn, WAVE_MAPPER, &WaveForm, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
	WaveHdr.dwBufferLength = len;
	WaveHdr.lpData = (LPSTR)pReordBuf;
	WaveHdr.dwFlags = 0;
	WaveHdr.dwUser = 0;
	WaveHdr.dwLoops = 1;
	WaveHdr.dwBytesRecorded = 0;
	waveInPrepareHeader(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn, &WaveHdr, sizeof(WAVEHDR));
	waveInStart(hWaveIn);
	Sleep(1000);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);
}

void AI_ivw_proc(const char *grammar_list, const char* audio_filename, const char* session_begin_params , void *buf, int len)
{
	const char *session_id = NULL;
	int err_code = MSP_SUCCESS;
	FILE *f_aud = NULL;
	long audio_size = 0;
	long real_read = 0;
	long audio_count = 0;
	int count = 0;
	int audio_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	char *audio_buffer = NULL;
	char sse_hints[128];
	if (NULL == audio_filename)
	{
		_tprintf(_T("*.[audio\\awake.pcm]文件传递为NULL!\n"));
		return;
	}

	/*f_aud = fopen(audio_filename, "rb");
	if (NULL == f_aud)
	{
		_tprintf(_T("*.[audio\\awake.pcm]文件打开失败!\n"));
		return;
	}
	fseek(f_aud, 0, SEEK_END);
	audio_size = ftell(f_aud);
	fseek(f_aud, 0, SEEK_SET);
	audio_buffer = (char *)malloc(audio_size);
	if (NULL == audio_buffer)
	{
		_tprintf(_T("*.内存申请失败,请确认内存是否充足!\n"));
		goto exit;
	}
	real_read = fread((void *)audio_buffer, 1, audio_size, f_aud);
	if (real_read != audio_size)
	{
		_tprintf(_T("*.[audio\\awake.pcm]文件内容读取失败!\n"));
		goto exit;
	}
*/

	session_id = QIVWSessionBegin(grammar_list, session_begin_params, &err_code);
	if (err_code != MSP_SUCCESS)
	{
		_tprintf(_T("*.AI处理函数[QIVWSessionBegin]执行失败,错误代码<%d>!\n"), err_code);
		goto exit;
	}

	err_code = QIVWRegisterNotify(session_id, AI_ivw_msg_proc, NULL);
	if (err_code != MSP_SUCCESS)
	{
		_snprintf(sse_hints, sizeof(sse_hints), "QIVWRegisterNotify errorCode=%d", err_code);
		_tprintf(_T("*.AI处理函数[QIVWRegisterNotify]执行失败,错误代码<%d>!\n"), err_code);
		goto exit;
	}
	err_code = QIVWAudioWrite(session_id, buf, len, audio_stat);
	if (MSP_SUCCESS != err_code)
	{
		_tprintf(_T("*.AI处理函数[QIVWAudioWrite]执行失败,错误代码<%d>!\n"), err_code);
		_snprintf(sse_hints, sizeof(sse_hints), "QIVWAudioWrite errorCode=%d", err_code);
		goto exit;
	}

exit:
	if (NULL != session_id)
	{
		QIVWSessionEnd(session_id, sse_hints);
	}
	if (NULL != f_aud)
	{
		fclose(f_aud);
	}
	if (NULL != audio_buffer)
	{
		free(audio_buffer);
	}
}

int AI_ivw_msg_proc(const char *sessionID, int msg, int param1, int param2, const void *info, void *userData)
{
	if (MSP_IVW_MSG_ERROR == msg) //唤醒出错消息
	{
		printf("*.AI唤醒出错 错误代码<%d>\n", param1);
	}
	else if (MSP_IVW_MSG_WAKEUP == msg) //唤醒成功消息
	{
		printf("*.AI成功唤醒 唤醒消息{ %s }\n", (char*)info);
		pSay_Text = const_cast<char*>("主人,我在,有什么需要伊卡洛斯帮助的吗?");
		_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)AI_Say, NULL, NULL, NULL);
	}
	return 0;
}
void GetRunPath(char* RunPathBuf, int slen)
{
	GetModuleFileNameA(GetModuleHandle(NULL), RunPathBuf, slen);
	char* pchar = strrchr(RunPathBuf, '\\');
	if (pchar)pchar[1] = 0;
}

void AI_Say()
{
	const char* session_begin_params = "voice_name = x_xiaofang, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
	const char* filename = "audio\\tts_Say.wav"; //合成的语音文件名称
	int ret = text_to_speech(pSay_Text, filename, session_begin_params);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI网络发声失败,错误代码<%d>\n"), ret);
	}
	printf("*.AI正在说< %s >\n", pSay_Text);
	pSay_Text = nullptr;
	char Path[256];
	GetRunPath(Path, sizeof(Path));
	strcat(Path, filename);
	PlaySoundA(Path, NULL, SND_FILENAME | SND_SYNC);
	_tprintf(_T("*.AI话语发声完毕\n"), pSay_Text);
	IsAwaken = true;
}
int text_to_speech(const char* src_text, const char* des_path, const char* params)
{
	int          ret = -1;
	FILE*        fp = NULL;
	const char*  sessionID = NULL;
	unsigned int audio_len = 0;
	wave_pcm_hdr wav_hdr = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

	if (NULL == src_text || NULL == des_path)
	{
		_tprintf(_T("*.AI网络发声失败,AI发声文本为NULL.\n"), ret);
		return ret;
	}
	fp = fopen(des_path, "wb");
	if (NULL == fp)
	{
		printf("*.AI网络发声失败,打开[ %s ]文件失败.\n", des_path);
		return ret;
	}
	/* 开始合成 */
	sessionID = QTTSSessionBegin(params, &ret);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI处理函数[QTTSSessionBegin]执行失败,错误代码<%d>!\n"), ret);
		fclose(fp);
		return ret;
	}
	ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), NULL);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI处理函数[QTTSTextPut]执行失败,错误代码<%d>!\n"), ret);
		QTTSSessionEnd(sessionID, "TextPutError");
		fclose(fp);
		return ret;
	}
	_tprintf(_T("*.AI正在思考说些什么"), ret);
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp); //添加wav音频头，使用采样率为16000
	while (1)
	{
		/* 获取合成音频 */
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if (MSP_SUCCESS != ret)
			break;
		if (NULL != data)
		{
			fwrite(data, audio_len, 1, fp);
			wav_hdr.data_size += audio_len; //计算data_size大小
		}
		if (MSP_TTS_FLAG_DATA_END == synth_status)
			break;
		_tprintf(_T("."));
		Sleep(200); //防止频繁占用CPU
	}
	_tprintf(_T("\n"));
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI处理函数[QTTSAudioGet]执行失败,错误代码<%d>!\n"), ret);
		QTTSSessionEnd(sessionID, "AudioGetError");
		fclose(fp);
		return ret;
	}
	/* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);

	/* 将修正过的数据写回文件头部,音频文件为wav格式 */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8, sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
	fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
	fwrite(&wav_hdr.data_size, sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值
	fclose(fp);
	fp = NULL;
	/* 合成完毕 */
	ret = QTTSSessionEnd(sessionID, "Normal");
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI处理函数[QTTSSessionEnd]执行失败,错误代码<%d>!\n"), ret);
	}

	return ret;
}