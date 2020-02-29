#pragma once
#ifndef Text_H
#define Text_H
#include <windows.h>
#include <time.h>

class Mod_Text {
public:
	Mod_Text();
	~Mod_Text();
	//取出文本左边(字符串缓冲区,取出长度);
	char *GetTextLeft(const char *TextBuff, UINT TakeLen);
	//取出文本左边(字符串缓冲区,取出长度);
	wchar_t *GetTextLeft(const wchar_t *TextBuff, UINT TakeLen);
	//取出文本右边(字符串缓冲区,取出长度);
	char *GetTextRight(const char *TextBuff, UINT TakeLen);
	//取出文本右边(字符串缓冲区,取出长度);
	wchar_t *GetTextRight(const wchar_t *TextBuff, UINT TakeLen);
	//取出文本中间内容(字符串缓冲区,左边文本,右边文本);
	char *GetTextMiddle(const char *TextBuff, const char *TextLeft, const char *TextRight);
	//取出文本中间内容(字符串缓冲区,左边文本,右边文本);
	wchar_t *GetTextMiddle(const wchar_t *TextBuff, const wchar_t *TextLeft, const wchar_t *TextRight);
	//文本替换(字符串缓冲区,搜索文本,预替换文本);
	char *TextReplace(const char *TextBuff, const char *SearchText, const char *Replacestrbuf);
	//文本替换(字符串缓冲区,搜索文本,预替换文本);
	wchar_t *TextReplace(const wchar_t *TextBuff, const wchar_t *SearchText, const wchar_t *Replacestrbuf);
	//文本转大写(字符串缓冲区);
	void TextToUpper(char *TextBuff);
	//文本转大写(字符串缓冲区);
	void TextToUpper(wchar_t *TextBuff);
	//文本转小写(字符串缓冲区);
	void TextToLower(char *TextBuff);
	//文本转小写(字符串缓冲区);
	void TextToLower(wchar_t *TextBuff);
	//随机字符串(需要的字符数目);
	char *RandText(UINT RandNumber);
	//随机字符串(需要的字符数目);
	wchar_t *RandTextW(UINT RandNumber);
	//随机汉字字符串(需要的汉字数目)
	char *RandChineseText(UINT RandNumber);
	//随机汉字字符串(需要的汉字数目)
	wchar_t *RandChineseTextW(UINT RandNumber);
private:
	void *TmpAlloc = nullptr;
};
#endif
