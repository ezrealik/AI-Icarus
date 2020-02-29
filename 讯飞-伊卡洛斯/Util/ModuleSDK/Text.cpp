#include "Text.h"
#pragma warning(disable:4996)

Mod_Text::Mod_Text() {
	srand((UINT)GetTickCount64());
}
Mod_Text::~Mod_Text() {
	if (TmpAlloc)free(TmpAlloc);
}
//取出文本左边(字符串缓冲区,取出长度);
char *Mod_Text::GetTextLeft(const char *TextBuff, UINT TakeLen) {
	if (!TextBuff)return nullptr;
	void *pTmpp = nullptr;
	if (TmpAlloc) { 
		UINT NewTmpLen = strlen(TextBuff) * sizeof(char);
		pTmpp = malloc(NewTmpLen);
		if (!pTmpp)return nullptr;
		memset(pTmpp, NULL, NewTmpLen);
		CopyMemory(pTmpp, TextBuff, NewTmpLen);
		free(TmpAlloc); 
		TmpAlloc = nullptr;
	}
	else pTmpp = (void *)TextBuff;
	UINT sSize = TakeLen * sizeof(char);
	TmpAlloc = malloc(sSize + sizeof(char));
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, sSize + sizeof(char));
	CopyMemory(TmpAlloc, pTmpp, sSize);
	return (char*)TmpAlloc;
}
//取出文本左边(字符串缓冲区,取出长度);
wchar_t *Mod_Text::GetTextLeft(const wchar_t *TextBuff, UINT TakeLen) {
	if (!TextBuff)return nullptr;
	void *pTmpp = nullptr;
	if (TmpAlloc) {
		UINT NewTmpLen = wcslen(TextBuff) * sizeof(wchar_t);
		pTmpp = malloc(NewTmpLen);
		if (!pTmpp)return nullptr;
		ZeroMemory(pTmpp, NewTmpLen);
		CopyMemory(pTmpp, TextBuff, NewTmpLen);
		free(TmpAlloc);
		TmpAlloc = nullptr;
	}
	else pTmpp = (void *)TextBuff;
	UINT sSize = TakeLen * sizeof(wchar_t);
	TmpAlloc = malloc(sSize + sizeof(wchar_t));
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, sSize + sizeof(wchar_t));
	CopyMemory(TmpAlloc, pTmpp, sSize);
	return (wchar_t*)TmpAlloc;
}
//取出文本右边(字符串缓冲区,取出长度);
char *Mod_Text::GetTextRight(const char *TextBuff, UINT TakeLen) {
	if (!TextBuff)return nullptr;
	void *pTmpp = nullptr;
	if (TmpAlloc) {
		UINT NewTmpLen = strlen(TextBuff) * sizeof(char);
		pTmpp = malloc(NewTmpLen);
		if (!pTmpp)return nullptr;
		ZeroMemory(pTmpp, NewTmpLen);
		CopyMemory(pTmpp, TextBuff, NewTmpLen);
		free(TmpAlloc);
		TmpAlloc = nullptr;
	}
	else pTmpp = (void *)TextBuff;
	UINT slen = strlen((char*)pTmpp) * sizeof(char), sSize = TakeLen * sizeof(char);
	TmpAlloc = malloc(sSize + sizeof(char));
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, sSize + sizeof(char));
	char *pTmp = (char*)pTmpp + (slen - sSize);
	CopyMemory(TmpAlloc, pTmp, sSize);
	return (char*)TmpAlloc;
}
//取出文本右边(字符串缓冲区,取出长度);
wchar_t *Mod_Text::GetTextRight(const wchar_t *TextBuff, UINT TakeLen) {
	if (!TextBuff)return nullptr;
	void *pTmpp = nullptr;
	if (TmpAlloc) {
		UINT NewTmpLen = wcslen(TextBuff) * sizeof(wchar_t);
		pTmpp = malloc(NewTmpLen);
		if (!pTmpp)return nullptr;
		ZeroMemory(pTmpp, NewTmpLen);
		CopyMemory(pTmpp, TextBuff, NewTmpLen);
		free(TmpAlloc);
		TmpAlloc = nullptr;
	}
	else pTmpp = (void *)TextBuff;
	UINT slen = wcslen((wchar_t*)pTmpp) * sizeof(wchar_t), sSize = TakeLen * sizeof(wchar_t);
	TmpAlloc = malloc(sSize + sizeof(wchar_t));
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, sSize + sizeof(wchar_t));
	char *pTmp = (char*)pTmpp + (slen - sSize);
	CopyMemory(TmpAlloc, pTmp, sSize);
	return (wchar_t*)TmpAlloc;
}
//取出文本中间内容(字符串缓冲区,左边文本,右边文本);
char *Mod_Text::GetTextMiddle(const char *TextBuff, const char *TextLeft, const char *TextRight) {
	if (!TextBuff)return nullptr;
	char *pTmpTextBuff = nullptr;
	UINT sLen = 0;//记录找到的文本长度
	BOOL IsTmpAlloc = FALSE;//是否申请过临时内存
	//判断之前是否已经申请过内存,如果有则申请临时存储然后释放;
	if (TmpAlloc) {
		UINT NewTmpLen = strlen(TextBuff) * sizeof(char) + sizeof(char);
		pTmpTextBuff = (char*)malloc(NewTmpLen);
		if (!pTmpTextBuff)return nullptr;
		ZeroMemory(pTmpTextBuff, NewTmpLen);
		CopyMemory(pTmpTextBuff, TextBuff, NewTmpLen);
		IsTmpAlloc = TRUE;
		free(TmpAlloc);
		TmpAlloc = nullptr;
	}
	else pTmpTextBuff = (char*)TextBuff;
	//寻找左边文本位置;
	char *pTmpl = strstr(pTmpTextBuff, TextLeft);
	if (!pTmpl)return nullptr;
	//如果找到记录文本位置,并移动到左边文本后面;
	char *pTmplpos = (char*)pTmpl + (strlen(TextLeft) * sizeof(char));
	//寻找右边文本位置;
	char *pTmpr = strstr(pTmplpos, TextRight);
	//判断是否找到右边文本,如果找到并且不与左边文本位置重复,则取长度;
	if (pTmpr != pTmplpos && pTmpr != NULL)sLen = (UINT)pTmpr - (UINT)pTmplpos;
	else sLen = strlen(pTmplpos) * sizeof(char);
	//申请存储取出文本的缓冲区;
	TmpAlloc = malloc(sLen + sizeof(char));
	//如果申请失败,判断有无申请内存,有则释放;
	if (!TmpAlloc) { if (IsTmpAlloc)free(pTmpTextBuff); return nullptr; }
	ZeroMemory(TmpAlloc, sLen + sizeof(char));
	//复制需要取出的文本到临时存储区;
	CopyMemory(TmpAlloc, pTmplpos, sLen);
	if (IsTmpAlloc)free(pTmpTextBuff);
	return (char*)TmpAlloc;
}
//取出文本中间内容(字符串缓冲区,左边文本,右边文本);
wchar_t *Mod_Text::GetTextMiddle(const wchar_t *TextBuff, const wchar_t *TextLeft, const wchar_t *TextRight) {
	if (!TextBuff)return nullptr;
	wchar_t *pTmpTextBuff = nullptr;
	UINT sLen = 0;//记录找到的文本长度
	BOOL IsTmpAlloc = FALSE;//是否申请过临时内存
	//判断之前是否已经申请过内存,如果有则申请临时存储然后释放;
	if (TmpAlloc) {
		UINT NewTmpLen = wcslen(TextBuff) * sizeof(wchar_t) + sizeof(wchar_t);
		pTmpTextBuff = (wchar_t*)malloc(NewTmpLen);
		if (!pTmpTextBuff)return nullptr;
		ZeroMemory(pTmpTextBuff, NewTmpLen);
		CopyMemory(pTmpTextBuff, TextBuff, NewTmpLen);
		IsTmpAlloc = TRUE;
		free(TmpAlloc);
		TmpAlloc = nullptr;
	}
	else pTmpTextBuff = (wchar_t*)TextBuff;
	//寻找左边文本位置;
	wchar_t *pTmpl = wcsstr(pTmpTextBuff, TextLeft);
	if (!pTmpl)return nullptr;
	//如果找到记录文本位置,并移动到左边文本后面;
	wchar_t *pTmplpos = (wchar_t*)((char*)pTmpl + (wcslen(TextLeft) * sizeof(wchar_t)));
	//寻找右边文本位置;
	wchar_t *pTmpr = wcsstr(pTmplpos, TextRight);
	//判断是否找到右边文本,如果找到并且不与左边文本位置重复,则取长度;
	if (pTmpr != pTmplpos && pTmpr != NULL)sLen = (UINT)pTmpr - (UINT)pTmplpos;
	else sLen = wcslen(pTmplpos) * sizeof(wchar_t);
	//申请存储取出文本的缓冲区;
	TmpAlloc = malloc(sLen + sizeof(wchar_t));
	//如果申请失败,判断有无申请内存,有则释放;
	if (!TmpAlloc) { if (IsTmpAlloc)free(pTmpTextBuff); return nullptr; }
	ZeroMemory(TmpAlloc, sLen + sizeof(wchar_t));
	//复制需要取出的文本到临时存储区;
	CopyMemory(TmpAlloc, pTmplpos, sLen);
	if (IsTmpAlloc)free(pTmpTextBuff);
	return (wchar_t*)TmpAlloc;
}
//文本替换(字符串缓冲区,搜索文本,预替换文本);
char *Mod_Text::TextReplace(const char *TextBuff, const char *SearchText, const char *Replacestrbuf) {
	if (!TextBuff)return false;
	void *pTmpp = nullptr;
	if (TmpAlloc) {
		UINT NewTmpLen = (strlen(TextBuff) * sizeof(char)) + sizeof(char);
		pTmpp = malloc(NewTmpLen);
		if (!pTmpp)return nullptr;
		ZeroMemory(pTmpp, NewTmpLen);
		CopyMemory(pTmpp, TextBuff, NewTmpLen);
		free(TmpAlloc);
		TmpAlloc = nullptr;
	}
	else pTmpp = (void *)TextBuff;
	UINT SrcTxtLen = strlen((char*)pTmpp), ReplaceTxtlen = strlen(Replacestrbuf);
	UINT SrcSize = SrcTxtLen * sizeof(char), ReplaceSize = ReplaceTxtlen * sizeof(char);
	char *pcSearch = strstr((char*)pTmpp, SearchText);
	if (!pcSearch)return (char*)pTmpp;
	UINT Lstrpos = (UINT)pcSearch - (UINT)pTmpp;
	TmpAlloc = malloc(SrcSize + ReplaceSize + sizeof(char));
	if (!TmpAlloc)return nullptr;
	void *Tmppp = TmpAlloc;
	ZeroMemory(Tmppp, SrcSize + ReplaceSize + sizeof(char));
	CopyMemory(Tmppp, pTmpp, Lstrpos);
	Tmppp = (char*)Tmppp + Lstrpos;
	CopyMemory(Tmppp, Replacestrbuf, ReplaceSize);
	Tmppp = (char*)Tmppp + ReplaceSize;
	char *pch = (char*)pcSearch + (strlen(SearchText) * sizeof(char));
	UINT CopyLen = strlen(pch) * sizeof(char);
	CopyMemory(Tmppp, pch, CopyLen);
	Tmppp = (char*)Tmppp + CopyLen;
	Tmppp = (char*)Tmppp + sizeof(char);
	ZeroMemory(Tmppp, 1);
	return (char*)TmpAlloc;
}
//文本替换(字符串缓冲区,搜索文本,预替换文本);
wchar_t *Mod_Text::TextReplace(const wchar_t *TextBuff, const wchar_t *SearchText, const wchar_t *Replacestrbuf) {
	if (!TextBuff)return false;
	void *pTmpp = nullptr;
	if (TmpAlloc) {
		UINT NewTmpLen = (wcslen(TextBuff) * sizeof(wchar_t)) + sizeof(wchar_t);
		pTmpp = malloc(NewTmpLen);
		if (!pTmpp)return nullptr;
		ZeroMemory(pTmpp, NewTmpLen);
		CopyMemory(pTmpp, TextBuff, NewTmpLen);
		free(TmpAlloc);
		TmpAlloc = nullptr;
	}
	else pTmpp = (void *)TextBuff;
	UINT SrcTxtLen = wcslen((wchar_t*)pTmpp), ReplaceTxtlen = wcslen(Replacestrbuf);
	UINT SrcSize = SrcTxtLen * sizeof(wchar_t), ReplaceSize = ReplaceTxtlen * sizeof(wchar_t);
	void *pcSearch = wcsstr((wchar_t*)pTmpp, SearchText);
	if (!pcSearch)return (wchar_t*)pTmpp;
	UINT Lstrpos = (UINT)pcSearch - (UINT)pTmpp;
	TmpAlloc = malloc(SrcSize + ReplaceSize + sizeof(wchar_t));
	if (!TmpAlloc)return nullptr;
	void *Tmppp = TmpAlloc;
	ZeroMemory(Tmppp, SrcSize + ReplaceSize + sizeof(wchar_t));
	CopyMemory(Tmppp, pTmpp, Lstrpos);
	Tmppp = (char*)Tmppp + Lstrpos;
	CopyMemory(Tmppp, Replacestrbuf, ReplaceSize);
	Tmppp = (char*)Tmppp + ReplaceSize;
	void *pch = (char*)pcSearch + (wcslen(SearchText) * sizeof(wchar_t));
	UINT CopyLen = wcslen((wchar_t*)pch) * sizeof(wchar_t);
	CopyMemory(Tmppp, pch, CopyLen);
	Tmppp = (char*)Tmppp + CopyLen;
	Tmppp = (char*)Tmppp + sizeof(wchar_t);
	ZeroMemory(Tmppp, 1);
	return (wchar_t*)TmpAlloc;
}
//文本转大写(字符串缓冲区);
void Mod_Text::TextToUpper(char *TextBuff) {
	if (!TextBuff)return;
	UINT sLen = strlen(TextBuff);
	for (UINT i = 0; i < sLen; i++) {
		if (TextBuff[i] >= 'a'&&TextBuff[i] <= 'z') {
			TextBuff[i] = TextBuff[i] + ('A' - 'a');
		}
	}
}
//文本转大写(字符串缓冲区);
void Mod_Text::TextToUpper(wchar_t *TextBuff) {
	if (!TextBuff)return;
	UINT sLen = wcslen(TextBuff);
	for (UINT i = 0; i < sLen; i++) {
		if (TextBuff[i] >= 'a'&&TextBuff[i] <= 'z') {
			TextBuff[i] = TextBuff[i] + ('A' - 'a');
		}
	}
}
//文本转小写(字符串缓冲区);
void Mod_Text::TextToLower(char *TextBuff) {
	if (!TextBuff)return;
	UINT sLen = strlen(TextBuff);
	for (UINT i = 0; i < sLen; i++) {
		if (TextBuff[i] >= 'A'&&TextBuff[i] <= 'Z') {
			TextBuff[i] = TextBuff[i] + ('a' - 'A');
		}
	}
}
//文本转小写(字符串缓冲区);
void Mod_Text::TextToLower(wchar_t *TextBuff) {
	if (!TextBuff)return;
	UINT sLen = wcslen(TextBuff);
	for (UINT i = 0; i < sLen; i++) {
		if (TextBuff[i] >= 'A'&&TextBuff[i] <= 'Z') {
			TextBuff[i] = TextBuff[i] + ('a' - 'A');
		}
	}
}
//随机数字字母字符串(需要的字符数目);
char *Mod_Text::RandText(UINT RandNumber) {
	if (TmpAlloc) { free(TmpAlloc); TmpAlloc = nullptr; }
	UINT TmpLen = RandNumber * sizeof(char) + sizeof(char);
	TmpAlloc = malloc(TmpLen);
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, TmpLen);
	char *pTmp = (char*)TmpAlloc;
	for (UINT i = 0; i < RandNumber; i++) {
		UINT rint = rand() % 3;
		switch (rint)
		{
		case 0:
			//pTmp[i] = (rand() % ((0xF7 - 0xB0) + 1) + 0xB0);
			pTmp[i] = rand() % (('9' - '0') + 1) + '0';
			break;
		case 1:
			pTmp[i] = rand() % (('z' - 'a') + 1) + 'a';
			break;
		case 2:
			pTmp[i] = rand() % (('Z' - 'A') + 1) + 'A';
			break;
		default:
			break;
		}
	}
	return (char*)TmpAlloc;
}
//随机数字字母字符串(需要的字符数目);
wchar_t *Mod_Text::RandTextW(UINT RandNumber) {
	if (TmpAlloc) { free(TmpAlloc); TmpAlloc = nullptr; }
	UINT TmpLen = RandNumber * sizeof(wchar_t) + sizeof(wchar_t);
	TmpAlloc = malloc(TmpLen);
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, TmpLen);
	wchar_t *pTmp = (wchar_t*)TmpAlloc;
	for (UINT i = 0; i < RandNumber; i++) {
		UINT rint = rand() % 3;
		switch (rint)
		{
		case 0:
			pTmp[i] = rand() % (('9' - '0') + 1) + '0';
			break;
		case 1:
			pTmp[i] = rand() % (('z' - 'a') + 1) + 'a';
			break;
		case 2:
			pTmp[i] = rand() % (('Z' - 'A') + 1) + 'A';
			break;
		default:
			break;
		}
	}
	return (wchar_t*)TmpAlloc;
}
//随机汉字字符串(需要的汉字数目)
char *Mod_Text::RandChineseText(UINT RandNumber) {
	if (TmpAlloc) { free(TmpAlloc); TmpAlloc = nullptr; }
	UINT TmpLen = RandNumber * (sizeof(char) + sizeof(char)) + sizeof(char);
	TmpAlloc = malloc(TmpLen);
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, TmpLen);
	char *pTmp = (char*)&TmpAlloc;
	//ANSI:汉字区的内码范围高字节从B0-F7,低字节从A1-FE;
	UINT cout = 0;
	for (UINT i = 0; i < RandNumber; i++) {
		pTmp[cout] = rand() % ((0xF7 - 0xB0) + 1) + 0xB0;
		cout++;
		pTmp[cout] = rand() % ((0xFE - 0xA1) + 1) + 0xA1;
		cout++;

	}
	return (char*)TmpAlloc;
}
//随机汉字字符串(需要的汉字数目)
wchar_t *Mod_Text::RandChineseTextW(UINT RandNumber) {
	if (TmpAlloc) { free(TmpAlloc); TmpAlloc = nullptr; }
	UINT TmpLen = RandNumber * sizeof(wchar_t) + sizeof(wchar_t);
	TmpAlloc = malloc(TmpLen);
	if (!TmpAlloc)return nullptr;
	ZeroMemory(TmpAlloc, TmpLen);
	wchar_t *pTmp = (wchar_t *)TmpAlloc;
	//Unicode 汉字内码的汉字区为4E00-9FA5, 共有20902个汉字
	for (UINT i = 0; i < RandNumber; i++) {
		pTmp[i] = rand() % ((0x9FA5 - 0x4E00) + 1) + 0x4E00;
	}
	return (wchar_t*)TmpAlloc;
}

