#include "AI_text_to_speech.h"

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

int _stdcall text_to_speech(const char* src_text, const char* des_path, const char* params)
{
	int          ret = -1;
	FILE*        fp = NULL;
	const char*  sessionID = NULL;
	unsigned int audio_len = 0;
	wave_pcm_hdr wav_hdr = default_wav_hdr;
	int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
	const char* session_begin_param = "voice_name = x_xiaofang, text_encoding = gb2312, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";
	if (!params)params = session_begin_param;
	if (NULL == src_text || NULL == des_path)
	{
		_tprintf(_T("*.AI网络发声失败,AI发声文本为NULL.\n"));
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
	//_tprintf(_T("*.AI正在思考说些什么"));
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
		//_tprintf(_T("."));
		Sleep(20); //防止频繁占用CPU
	}
	//_tprintf(_T("\n"));
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

void _stdcall Play_AI_Speech(const char* src_text, const char* des_path, bool IsASYNC)
{
	SYSTEMTIME ptime;
	GetLocalTime(&ptime);
	int ret = text_to_speech(src_text, des_path, NULL);
	if (MSP_SUCCESS != ret)
	{
		_tprintf(_T("*.AI网络发声失败,错误代码<%d>\n"), ret);
	}
	char Path[256];
	GetRunPath(Path, sizeof(Path));
	strcat(Path, des_path);
	printf("*.AI正在说< %s >\n", src_text);
	if (IsASYNC) {
		PlaySoundA(Path, NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
		PlaySoundA(Path, NULL, SND_FILENAME | SND_SYNC);
	}
	_tprintf(_T("*.AI话语发声完毕\n"));
}