#include "lib_http.h"

namespace lib_http
{
	http::http()
	{
		CoInitialize(NULL);
		m_pScriptControlPtr.CreateInstance(__uuidof(ScriptControl));
	}

	http::~http()
	{
		m_pScriptControlPtr->Release();
		CoUninitialize();
	}
	std::string _fastcall http::GetHost(std::string url)
	{
		bool Ishttps;
		url.find("https://") == std::string::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find("https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find("http://") + (sizeof("http://") - sizeof(char));
		url.find('/', strpos) == std::string::npos ? coutof = url.find('/', strpos) : coutof = url.find('/', strpos);
		if (Ishttps)return url.substr(strpos, coutof - (sizeof("https://") - sizeof(char)));
		return url.substr(strpos, coutof - (sizeof("http://") - sizeof(char)));
	}
	std::wstring _fastcall http::GetHost(std::wstring url)
	{
		bool Ishttps;
		url.find(L"https://") == std::wstring::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find(L"https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find(L"http://") + (sizeof(L"http://") - sizeof(char));
		url.find('/', strpos) == std::wstring::npos ? coutof = url.find('/', strpos) : coutof = url.find('/', strpos);
		if (Ishttps)return url.substr(strpos, coutof - (sizeof("https://") - sizeof(char)));
		return url.substr(strpos, coutof - (sizeof("http://") - sizeof(char)));
	}
	std::string _fastcall http::GetURLPage(std::string url)
	{
		bool Ishttps;
		url.find("https://") == std::string::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find("https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find("http://") + (sizeof("http://") - sizeof(char));
		coutof = url.find('/', strpos);
		if (coutof == std::string::npos)return "";
		return url.substr(coutof + sizeof('/'), url.length() - (coutof + sizeof('/')));
	}
	std::wstring _fastcall http::GetURLPage(std::wstring url)
	{
		bool Ishttps;
		url.find(L"https://") == std::wstring::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find(L"https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find(L"http://") + (sizeof("http://") - sizeof(char));
		coutof = url.find('/', strpos);
		if (coutof == std::wstring::npos)return L"";
		return url.substr(coutof + sizeof('/'), url.length() - (coutof + sizeof('/')));
	}
	bool _fastcall http::POST(std::string url, std::string &ResultData, std::string RequestData, std::string Header, std::string Cookies, std::string ReturnCookies)
	{
		bool IsHttps;
		//把url转小写(不使用)
		//std::transform(url.begin(), url.end(), url.begin(), ::tolower);
		url.find("https") == std::string::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		if (Header.find("User-Agent:") == std::string::npos)Header.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find("Accept:") == std::string::npos)Header.append("Accept: */*\r\n");
		if (Header.find("Accept-Language:") == std::string::npos)Header.append("Accept-Language: zh-cn\r\n");
		if (Header.find("Content-Type:") == std::string::npos)Header.append("Content-Type: application/x-www-form-urlencoded\r\n");
		if (Header.find("Host:") == std::string::npos) {
			Header.append("Host: ");
			Header.append(GetHost(url));
			Header.append("\r\n");
		}
		if (Header.find("Referer:") == std::string::npos) {
			Header.append("Referer: ");
			Header.append(url);
			Header.append("\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find("Cookies:") == std::string::npos) {
				Header.append("Cookies: ");
				Header.append(Cookies);
				Header.append("\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append("\r\n");
			}
		}
#pragma endregion
		//打开HTTP句柄;
		HINTERNET hInternetopen = InternetOpenA(Header.c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, NULL);
		if (!hInternetopen)return false;
		//访问端口;
		INTERNET_PORT InterPort;
		IsHttps ? InterPort = INTERNET_DEFAULT_HTTPS_PORT : InterPort = INTERNET_DEFAULT_HTTP_PORT;
		//创建Internet连接;
		HINTERNET hInternetConnect = InternetConnectA(hInternetopen, GetHost(url).c_str(), InterPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
		if (!hInternetConnect) { InternetCloseHandle(hInternetopen); return false; }
		DWORD RequestFlg = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;
		if (!Cookies.empty())RequestFlg = RequestFlg | INTERNET_FLAG_NO_COOKIES;
		IsHttps ? RequestFlg |= INTERNET_FLAG_SECURE : RequestFlg |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
		HINTERNET hInternetRequest = HttpOpenRequestA(hInternetConnect, "POST", GetURLPage(url).c_str(), "HTTP/1.1", NULL, NULL, RequestFlg, NULL);
		if (!hInternetRequest) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); return false; }
		if (!HttpSendRequestA(hInternetRequest, Header.c_str(), Header.length(), (LPVOID)(RequestData.c_str()), RequestData.length()))return false;
		//new的异常处理
		char *pResultData = nullptr;
		try { pResultData = new char[1025]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		UINT  ResultLen = 0;
		do
		{
			ZeroMemory(pResultData, 1025);
			InternetReadFile(hInternetRequest, pResultData, 1024, reinterpret_cast<LPDWORD>(&ResultLen));
			ResultData.append(reinterpret_cast<char*>(pResultData));
		} while (ResultLen);
		char *pTmpQuery = nullptr;
		try { pTmpQuery = new char[4096]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		ZeroMemory(pTmpQuery, 4096 * sizeof(char));
		DWORD CookiesLength = 4095;
		HttpQueryInfoA(hInternetRequest, HTTP_QUERY_SET_COOKIE, pTmpQuery, &CookiesLength, NULL);
		ReturnCookies.append(pTmpQuery);
		InternetCloseHandle(hInternetopen);
		InternetCloseHandle(hInternetConnect);
		InternetCloseHandle(hInternetRequest);
		delete[]pResultData;
		delete[]pTmpQuery;
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	bool _fastcall http::POST(std::wstring url, std::string &ResultData, std::wstring RequestData, std::wstring Header, std::wstring Cookies, std::wstring ReturnCookies)
	{
		bool IsHttps;
		url.find(L"https") == std::string::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		if (Header.find(L"User-Agent:") == std::string::npos)Header.append(L"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find(L"Accept:") == std::string::npos)Header.append(L"Accept: */*\r\n");
		if (Header.find(L"Accept-Language:") == std::string::npos)Header.append(L"Accept-Language: zh-cn\r\n");
		if (Header.find(L"Content-Type:") == std::string::npos)Header.append(L"Content-Type: application/x-www-form-urlencoded\r\n");
		if (Header.find(L"Host:") == std::string::npos) {
			Header.append(L"Host: ");
			Header.append(GetHost(url));
			Header.append(L"\r\n");
		}
		if (Header.find(L"Referer:") == std::string::npos) {
			Header.append(L"Referer: ");
			Header.append(url);
			Header.append(L"\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find(L"Cookies:") == std::string::npos) {
				Header.append(L"Cookies: ");
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
		}
#pragma endregion
		//打开HTTP句柄;
		HINTERNET hInternetopen = InternetOpenW(Header.c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, NULL);
		if (!hInternetopen)return false;
		//访问端口;
		INTERNET_PORT InterPort;
		IsHttps ? InterPort = INTERNET_DEFAULT_HTTPS_PORT : InterPort = INTERNET_DEFAULT_HTTP_PORT;
		//创建Internet连接;
		HINTERNET hInternetConnect = InternetConnectW(hInternetopen, GetHost(url).c_str(), InterPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
		if (!hInternetConnect) { InternetCloseHandle(hInternetopen); return false; }
		DWORD RequestFlg = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;
		if (!Cookies.empty())RequestFlg = RequestFlg | INTERNET_FLAG_NO_COOKIES;
		IsHttps ? RequestFlg |= INTERNET_FLAG_SECURE : RequestFlg |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
		HINTERNET hInternetRequest = HttpOpenRequestW(hInternetConnect, L"POST", GetURLPage(url).c_str(), L"HTTP/1.1", NULL, NULL, RequestFlg, NULL);
		if (!hInternetRequest) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); return false; }
		if (!HttpSendRequestW(hInternetRequest, Header.c_str(), Header.length(), (LPVOID)(RequestData.c_str()), RequestData.length()))return false;
		//new的异常处理
		char *pResultData = nullptr;
		try { pResultData = new char[1025]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		UINT  ResultLen = 0;
		do
		{
			ZeroMemory(pResultData, 1025);
			InternetReadFile(hInternetRequest, pResultData, 1024, reinterpret_cast<LPDWORD>(&ResultLen));
			ResultData.append(reinterpret_cast<char*>(pResultData));
		} while (ResultLen);
		wchar_t *pTmpQuery = nullptr;
		try { pTmpQuery = new wchar_t[4096]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		ZeroMemory(pTmpQuery, 4096 * sizeof(wchar_t));
		DWORD CookiesLength = 4095;
		HttpQueryInfoW(hInternetRequest, HTTP_QUERY_SET_COOKIE, pTmpQuery, &CookiesLength, NULL);
		ReturnCookies.append(pTmpQuery);
		InternetCloseHandle(hInternetopen);
		InternetCloseHandle(hInternetConnect);
		InternetCloseHandle(hInternetRequest);
		delete[]pResultData;
		delete[]pTmpQuery;
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	bool _fastcall http::GET(std::string url, std::string &ResultData, std::string Header, std::string Cookies, std::string ReturnCookies)
	{
		bool IsHttps;
		url.find("https") == std::string::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		if (Header.find("User-Agent:") == std::string::npos)Header.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find("Accept:") == std::string::npos)Header.append("Accept: */*\r\n");
		if (Header.find("Accept-Language:") == std::string::npos)Header.append("Accept-Language: zh-cn\r\n");
		if (Header.find("Content-Type:") == std::string::npos)Header.append("Content-Type: application/x-www-form-urlencoded\r\n");
		if (Header.find("Host:") == std::string::npos) {
			Header.append("Host: ");
			Header.append(GetHost(url));
			Header.append("\r\n");
		}
		if (Header.find("Referer:") == std::string::npos) {
			Header.append("Referer: ");
			Header.append(url);
			Header.append("\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find("Cookies:") == std::string::npos) {
				Header.append("Cookies: ");
				Header.append(Cookies);
				Header.append("\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append("\r\n");
			}
		}
#pragma endregion
		//打开HTTP句柄;
		HINTERNET hInternetopen = InternetOpenA(Header.c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, NULL);
		if (!hInternetopen)return false;
		//访问端口;
		INTERNET_PORT InterPort;
		IsHttps ? InterPort = INTERNET_DEFAULT_HTTPS_PORT : InterPort = INTERNET_DEFAULT_HTTP_PORT;
		//创建Internet连接;
		HINTERNET hInternetConnect = InternetConnectA(hInternetopen, GetHost(url).c_str(), InterPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
		if (!hInternetConnect) { InternetCloseHandle(hInternetopen); return false; }
		DWORD RequestFlg = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;
		if (!Cookies.empty())RequestFlg = RequestFlg | INTERNET_FLAG_NO_COOKIES;
		IsHttps ? RequestFlg |= INTERNET_FLAG_SECURE : RequestFlg |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
		HINTERNET hInternetRequest = HttpOpenRequestA(hInternetConnect, "GET", GetURLPage(url).c_str(), "HTTP/1.1", NULL, NULL, RequestFlg, NULL);
		if (!hInternetRequest) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); return false; }
		if (!HttpSendRequestA(hInternetRequest, Header.c_str(), Header.length(), nullptr, NULL))return false;
		//new的异常处理
		char *pResultData = nullptr;
		try { pResultData = new char[1025]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		UINT  ResultLen = 0;
		do
		{
			ZeroMemory(pResultData, 1025);
			InternetReadFile(hInternetRequest, pResultData, 1024, reinterpret_cast<LPDWORD>(&ResultLen));
			ResultData.append(reinterpret_cast<char*>(pResultData));
		} while (ResultLen);
		char *pTmpQuery = nullptr;
		try { pTmpQuery = new char[4096]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		ZeroMemory(pTmpQuery, 4096 * sizeof(char));
		DWORD CookiesLength = 4095;
		HttpQueryInfoA(hInternetRequest, HTTP_QUERY_SET_COOKIE, pTmpQuery, &CookiesLength, NULL);
		ReturnCookies.append(pTmpQuery);
		InternetCloseHandle(hInternetopen);
		InternetCloseHandle(hInternetConnect);
		InternetCloseHandle(hInternetRequest);
		delete[]pResultData;
		delete[]pTmpQuery;
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	bool _fastcall http::GET(std::wstring url, std::string &ResultData, std::wstring Header, std::wstring Cookies, std::wstring ReturnCookies)
	{
		bool IsHttps;
		url.find(L"https") == std::string::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		if (Header.find(L"User-Agent:") == std::string::npos)Header.append(L"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find(L"Accept:") == std::string::npos)Header.append(L"Accept: */*\r\n");
		if (Header.find(L"Accept-Language:") == std::string::npos)Header.append(L"Accept-Language: zh-cn\r\n");
		if (Header.find(L"Content-Type:") == std::string::npos)Header.append(L"Content-Type: application/x-www-form-urlencoded\r\n");
		if (Header.find(L"Host:") == std::string::npos) {
			Header.append(L"Host: ");
			Header.append(GetHost(url));
			Header.append(L"\r\n");
		}
		if (Header.find(L"Referer:") == std::string::npos) {
			Header.append(L"Referer: ");
			Header.append(url);
			Header.append(L"\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find(L"Cookies:") == std::string::npos) {
				Header.append(L"Cookies: ");
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
		}
#pragma endregion
		//打开HTTP句柄;
		HINTERNET hInternetopen = InternetOpenW(Header.c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, NULL);
		if (!hInternetopen)return false;
		//访问端口;
		INTERNET_PORT InterPort;
		IsHttps ? InterPort = INTERNET_DEFAULT_HTTPS_PORT : InterPort = INTERNET_DEFAULT_HTTP_PORT;
		//创建Internet连接;
		HINTERNET hInternetConnect = InternetConnectW(hInternetopen, GetHost(url).c_str(), InterPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
		if (!hInternetConnect) { InternetCloseHandle(hInternetopen); return false; }
		DWORD RequestFlg = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;
		if (!Cookies.empty())RequestFlg = RequestFlg | INTERNET_FLAG_NO_COOKIES;
		IsHttps ? RequestFlg |= INTERNET_FLAG_SECURE : RequestFlg |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
		HINTERNET hInternetRequest = HttpOpenRequestW(hInternetConnect, L"GET", GetURLPage(url).c_str(), L"HTTP/1.1", NULL, NULL, RequestFlg, NULL);
		if (!hInternetRequest) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); return false; }
		if (!HttpSendRequestW(hInternetRequest, Header.c_str(), Header.length(), nullptr, NULL))return false;
		//new的异常处理
		char *pResultData = nullptr;
		try { pResultData = new char[1025]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		UINT  ResultLen = 0;
		do
		{
			ZeroMemory(pResultData, 1025);
			InternetReadFile(hInternetRequest, pResultData, 1024, reinterpret_cast<LPDWORD>(&ResultLen));
			ResultData.append(reinterpret_cast<char*>(pResultData));
		} while (ResultLen);
		wchar_t *pTmpQuery = nullptr;
		try { pTmpQuery = new wchar_t[4096]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		ZeroMemory(pTmpQuery, 4096 * sizeof(wchar_t));
		DWORD CookiesLength = 4095;
		HttpQueryInfoW(hInternetRequest, HTTP_QUERY_SET_COOKIE, pTmpQuery, &CookiesLength, NULL);
		ReturnCookies.append(pTmpQuery);
		InternetCloseHandle(hInternetopen);
		InternetCloseHandle(hInternetConnect);
		InternetCloseHandle(hInternetRequest);
		delete[]pResultData;
		delete[]pTmpQuery;
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	std::string _fastcall http::GetLocalCookies(std::string host)
	{
		char *TmpCookies = nullptr;
		DWORD CookiesLen = 4097;
		try { TmpCookies = new char[CookiesLen]; }
		catch (...) { return ""; }
		ZeroMemory(TmpCookies, 4097);
		if (!InternetGetCookieA(host.c_str(), NULL, reinterpret_cast<LPSTR>(TmpCookies), &CookiesLen))return "";
		std::string Cookies(reinterpret_cast<LPSTR>(TmpCookies));
		return Cookies;
	}
	std::wstring _fastcall http::GetLocalCookies(std::wstring host)
	{
		char *TmpCookies = nullptr;
		DWORD CookiesLen = 4097;
		try { TmpCookies = new char[CookiesLen]; }
		catch (...) { return L""; }
		ZeroMemory(TmpCookies, 4097);
		if (!InternetGetCookieW(host.c_str(), NULL, reinterpret_cast<LPWSTR>(TmpCookies), &CookiesLen))return L"";
		std::wstring Cookies(reinterpret_cast<LPWSTR>(TmpCookies));
		return Cookies;
	}
	VARIANT _fastcall http::ExecuteJScript(std::string JScript, std::string FuncetionName, std::string ErrorLog)
	{
		std::stringstream loger;
		VARIANT Result = { 0 };
		try {
			m_pScriptControlPtr->Language = "JScript";
			m_pScriptControlPtr->ExecuteStatement(JScript.c_str());
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << "JScript加载错误!\r\n";
			loger << "错误代码: [" << ErrorCode << "]\r\n";
			loger << "出错位置: [" << ErrorLine << "]行\r\n";
			loger << "提示信息: " << e.Description() << "\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str("");
			return Result;
		}
		try {
			Result = m_pScriptControlPtr->Eval(FuncetionName.c_str());
			m_pScriptControlPtr->Reset();
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << "JScript运行错误!\r\n";
			loger << "错误代码: [" << ErrorCode << "]\r\n";
			loger << "出错位置: [" << ErrorLine << "]行\r\n";
			loger << "提示信息: " << e.Description() << "\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str("");
			return Result;
		}
		loger.clear();
		loger.str("");
		return Result;
	}
	VARIANT _fastcall http::ExecuteJScript(std::wstring JScript, std::wstring FuncetionName, std::wstring ErrorLog)
	{
		std::wstringstream loger;
		VARIANT Result = { 0 };
		try {
			m_pScriptControlPtr->Language = "JScript";
			m_pScriptControlPtr->ExecuteStatement(JScript.c_str());
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << L"JScript加载错误!\r\n";
			loger << L"错误代码: [" << ErrorCode << L"]\r\n";
			loger << L"出错位置: [" << ErrorLine << L"]行\r\n";
			loger << L"提示信息: " << e.Description() << L"\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str(L"");
			return Result;
		}
		try {
			Result = m_pScriptControlPtr->Eval(FuncetionName.c_str());
			m_pScriptControlPtr->Reset();
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << L"JScript运行错误!\r\n";
			loger << L"错误代码: [" << ErrorCode << L"]\r\n";
			loger << L"出错位置: [" << ErrorLine << L"]行\r\n";
			loger << L"提示信息: " << e.Description() << L"\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str(L"");
			return Result;
		}
		loger.clear();
		loger.str(L"");
		return Result;
	}
	bool _fastcall http::RegEx_FindString(std::string Regex, std::string DestString, bool IsCase /* = false */, bool IsMultiRows /* = true */, bool IsGlobal /* = true */)
	{
		return true;
	}
	bool _fastcall http::DownloadFile(std::string url, std::string FilePath, DownloadProc DownloadPr /* = nullptr */)
	{
		if (url.empty())return false;
		if (FilePath.empty())return false;
		bool IsHttps;
		url.find("https") == std::string::npos ? IsHttps = false : IsHttps = true;
		std::stringstream Header;
#pragma region 协议头处理
		Header << "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n";
		Header << "Accept: */*\r\n";
		Header << "Accept-Language: zh-cn\r\n";
		Header << "Content-Type: application/x-www-form-urlencoded\r\n";
		Header << "Host:" << GetHost(url) << "\r\n";
		Header << "Referer:" << url << "\r\n";
#pragma endregion
		//打开HTTP句柄;
		HINTERNET hInternetopen = InternetOpenA(Header.str().c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, NULL);
		if (!hInternetopen)return false;
		//访问端口;
		INTERNET_PORT InterPort;
		IsHttps ? InterPort = INTERNET_DEFAULT_HTTPS_PORT : InterPort = INTERNET_DEFAULT_HTTP_PORT;
		//创建Internet连接;
		HINTERNET hInternetConnect = InternetConnectA(hInternetopen, GetHost(url).c_str(), InterPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
		if (!hInternetConnect) { InternetCloseHandle(hInternetopen); return false; }
		DWORD RequestFlg = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;
		IsHttps ? RequestFlg |= INTERNET_FLAG_SECURE : RequestFlg |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
		HINTERNET hInternetRequest = HttpOpenRequestA(hInternetConnect, "GET", GetURLPage(url).c_str(), "HTTP/1.1", NULL, NULL, RequestFlg, NULL);
		if (!hInternetRequest) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); return false; }
		if (!HttpSendRequestA(hInternetRequest, Header.str().c_str(), Header.str().length(), nullptr, NULL))return false;
		//new的异常处理
		char *pResultData = nullptr;
		try { pResultData = new char[1025]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		UINT  ResultLen = 0;
		DWORD DownloadTickSize = 0, SecondTick = 0, DownloadSize = 0;
		time_t  ticksecond = time(NULL);
		std::ofstream file;
		file.open(FilePath, ios::out | ios::binary);
		char Tmpbuf[55] = { 0 };
		DWORD buflen = sizeof(Tmpbuf);
		HttpQueryInfo(hInternetRequest, HTTP_QUERY_CONTENT_LENGTH, Tmpbuf, &buflen, NULL);
		DWORD dwFileSize = atol(Tmpbuf);
		do
		{
			ZeroMemory(pResultData, 1025);
			InternetReadFile(hInternetRequest, pResultData, 1024, reinterpret_cast<LPDWORD>(&ResultLen));
			file.write(pResultData, ResultLen);
			DownloadTickSize += ResultLen;
			DownloadSize += ResultLen;
			if (time(NULL) - ticksecond > 1) {
				SecondTick = DownloadTickSize;
				DownloadTickSize = 0; 
				ticksecond = time(NULL);
			}
			if (DownloadPr) {
				if(!DownloadPr(dwFileSize, DownloadSize, SecondTick))break;
			}
		} while (ResultLen);
		InternetCloseHandle(hInternetopen);
		InternetCloseHandle(hInternetConnect);
		InternetCloseHandle(hInternetRequest);
		delete[]pResultData;
		file.close();
		Header.clear();
		Header.str("");
		return true;
	}
	bool _fastcall http::DownloadFile(std::wstring url, std::wstring FilePath, DownloadProc DownloadPr /* = nullptr */)
	{
		if (url.empty())return false;
		if (FilePath.empty())return false;
		bool IsHttps;
		url.find(L"https") == std::wstring::npos ? IsHttps = false : IsHttps = true;
		std::wstringstream Header;
#pragma region 协议头处理
		Header << "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n";
		Header << "Accept: */*\r\n";
		Header << "Accept-Language: zh-cn\r\n";
		Header << "Content-Type: application/x-www-form-urlencoded\r\n";
		Header << "Host:" << GetHost(url) << "\r\n";
		Header << "Referer:" << url << "\r\n";
#pragma endregion
		//打开HTTP句柄;
		HINTERNET hInternetopen = InternetOpenW(Header.str().c_str(), INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, NULL);
		if (!hInternetopen)return false;
		//访问端口;
		INTERNET_PORT InterPort;
		IsHttps ? InterPort = INTERNET_DEFAULT_HTTPS_PORT : InterPort = INTERNET_DEFAULT_HTTP_PORT;
		//创建Internet连接;
		HINTERNET hInternetConnect = InternetConnectW(hInternetopen, GetHost(url).c_str(), InterPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
		if (!hInternetConnect) { InternetCloseHandle(hInternetopen); return false; }
		DWORD RequestFlg = INTERNET_FLAG_RELOAD | INTERNET_COOKIE_THIRD_PARTY;
		IsHttps ? RequestFlg |= INTERNET_FLAG_SECURE : RequestFlg |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
		HINTERNET hInternetRequest = HttpOpenRequestW(hInternetConnect, L"GET", GetURLPage(url).c_str(), L"HTTP/1.1", NULL, NULL, RequestFlg, NULL);
		if (!hInternetRequest) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); return false; }
		char Tmpbuf[55] = { 0 };
		DWORD buflen = sizeof(Tmpbuf);
		HttpQueryInfo(hInternetRequest, HTTP_QUERY_CONTENT_RANGE, Tmpbuf, &buflen, NULL);
		DWORD dwFileSize = atol(Tmpbuf);
		if (!HttpSendRequestW(hInternetRequest, Header.str().c_str(), Header.str().length(), nullptr, NULL))return false;
		//new的异常处理
		char *pResultData = nullptr;
		try { pResultData = new char[1025]; }
		catch (...) { InternetCloseHandle(hInternetConnect); InternetCloseHandle(hInternetopen); InternetCloseHandle(hInternetRequest); return false; }
		UINT  ResultLen = 0;
		DWORD DownloadTickSize = 0, SecondTick = 0, DownloadSize = 0;
		time_t  ticksecond = time(NULL);
		std::ofstream file;
		file.open(FilePath, ios::out | ios::binary);
		do
		{
			ZeroMemory(pResultData, 1025);
			InternetReadFile(hInternetRequest, pResultData, 1024, reinterpret_cast<LPDWORD>(&ResultLen));
			file.write(pResultData, ResultLen);
			DownloadTickSize += ResultLen;
			DownloadSize += ResultLen;
			if (time(NULL) - ticksecond > 1) {
				SecondTick = DownloadTickSize;
				DownloadTickSize = 0;
				ticksecond = time(NULL);
			}
			if (DownloadPr) {
				if (!DownloadPr(dwFileSize, DownloadSize, SecondTick))break;
			}
		} while (ResultLen);
		InternetCloseHandle(hInternetopen);
		InternetCloseHandle(hInternetConnect);
		InternetCloseHandle(hInternetRequest);
		delete[]pResultData;
		file.close();
		Header.clear();
		Header.str(L"");
		return true;
	}
	//=================================================
	//http Socket请求方式
	http2::http2()
	{
		CoInitialize(NULL);
		m_pScriptControlPtr.CreateInstance(__uuidof(ScriptControl));
		WSAStartup(MAKEWORD(2, 2), &m_wsa) == 0 ? m_IsInitWSA = true : m_IsInitWSA = false;
	}
	http2::~http2()
	{
		m_pScriptControlPtr->Release();
		CoUninitialize();
	}
	std::string _fastcall http2::GetHost(std::string url)
	{
		bool Ishttps;
		url.find("https://") == std::string::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find("https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find("http://") + (sizeof("http://") - sizeof(char));
		url.find('/', strpos) == std::string::npos ? coutof = url.find('/', strpos) : coutof = url.find('/', strpos);
		if (Ishttps)return url.substr(strpos, coutof - (sizeof("https://") - sizeof(char)));
		return url.substr(strpos, coutof - (sizeof("http://") - sizeof(char)));
	}
	std::wstring _fastcall http2::GetHost(std::wstring url)
	{
		bool Ishttps;
		url.find(L"https://") == std::wstring::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find(L"https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find(L"http://") + (sizeof(L"http://") - sizeof(char));
		url.find('/', strpos) == std::wstring::npos ? coutof = url.find('/', strpos) : coutof = url.find('/', strpos);
		if (Ishttps)return url.substr(strpos, coutof - (sizeof("https://") - sizeof(char)));
		return url.substr(strpos, coutof - (sizeof("http://") - sizeof(char)));
	}
	std::string _fastcall http2::GetURLPage(std::string url)
	{
		bool Ishttps;
		url.find("https://") == std::string::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find("https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find("http://") + (sizeof("http://") - sizeof(char));
		coutof = url.find('/', strpos);
		if (coutof == std::string::npos)return "";
		/*std::string Result = url.substr(coutof + sizeof('/'), url.length() - (coutof + sizeof('/')));
		if (Result.empty() || coutof == std::string::npos)Result = "/";*/
		return url.substr(coutof + sizeof('/'), url.length() - (coutof + sizeof('/')));
	}
	std::wstring _fastcall http2::GetURLPage(std::wstring url)
	{
		bool Ishttps;
		url.find(L"https://") == std::wstring::npos ? Ishttps = false : Ishttps = true;
		size_t strpos, coutof;
		Ishttps ? strpos = url.find(L"https://") + (sizeof("https://") - sizeof(char)) : strpos = url.find(L"http://") + (sizeof("http://") - sizeof(char));
		coutof = url.find('/', strpos);
		if (coutof == std::wstring::npos)return L"";
		return url.substr(coutof + sizeof('/'), url.length() - (coutof + sizeof('/')));
	}
	std::string _fastcall http2::GetHttpData(std::string urldata)
	{
		std::string result;
		char *ptmp = const_cast<char*>(urldata.c_str());
		if (!ptmp)return "";
		for (unsigned int i=0;i<urldata.size();i++)
		{
			if (ptmp[i] == '\n'&&ptmp[i + 1] == '\r') {
				ptmp += i + 3;
				result = ptmp;
				break;
			}
			if (i + 3 > urldata.size())return "";
		}
		return result;
	}
	inline std::string GetCookies(std::string urldata)
	{
		string tmp;
		char *pchar = const_cast<char*>(urldata.c_str()), *pTmp = nullptr;
		pTmp = strtok(pchar, "\r\n");
		while (pTmp) {
			if (strstr(pTmp, "Set-Cookie")) {
				tmp.append(pTmp + sizeof("Set-Cookie:"));
			}
			pTmp = strtok(NULL, "\r\n");
		}
		return tmp;
	}
	bool _fastcall http2::POST(std::string url, std::string &ResultData, std::string RequestData, std::string Header, std::string Cookies, std::string ReturnCookies)
	{
		if (!m_IsInitWSA)return false;
		bool IsHttps;
		//把url转小写(不使用)
		//std::transform(url.begin(), url.end(), url.begin(), ::tolower);
		url.find("https") == std::string::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		std::string tmp = "POST /" + GetURLPage(url) + " HTTP/1.1\r\n";
		Header = tmp + Header;
		if (Header.find("User-Agent:") == std::string::npos)Header.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find("Accept:") == std::string::npos)Header.append("Accept:*/*\r\n");
		if (Header.find("Accept-Language:") == std::string::npos)Header.append("Accept-Language:zh-cn\r\n");
		if (Header.find("Content-Type:") == std::string::npos)Header.append("Content-Type:application/x-www-form-urlencoded\r\n");
		if (Header.find("Cache-Control:") == std::string::npos)Header.append("Cache-Control:no-cache\r\n");
		if (Header.find("Host:") == std::string::npos) {
			Header.append("Host:");
			Header.append(GetHost(url));
			Header.append("\r\n");
		}
		if (Header.find("Referer:") == std::string::npos) {
			Header.append("Referer:");
			Header.append(url);
			Header.append("\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find("Cookies:") == std::string::npos) {
				Header.append("Cookies:");
				Header.append(Cookies);
				Header.append("\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append("\r\n");
			}
		}
		char *Datalen = new char[256];
		sprintf(Datalen, "%d", RequestData.size());
		Header.append("Content-Length:");
		Header.append(Datalen);
		Header.append("\r\n");
#pragma endregion
		SOCKADDR_IN Sockaddr = { 0 };
		IsHttps ? Sockaddr.sin_port = htons(443) : Sockaddr.sin_port = htons(80);
		Sockaddr.sin_family = AF_INET;
		hostent *hostip = gethostbyname(GetHost(url).c_str());
		if (!hostip)return false;
		Sockaddr.sin_addr = *reinterpret_cast<in_addr*>(hostip->h_addr);
		SOCKET Client = socket(AF_INET, SOCK_STREAM, 0);
		if (Client == INVALID_SOCKET)return false;
		if (connect(Client, reinterpret_cast<SOCKADDR*>(&Sockaddr), sizeof(Sockaddr)) == SOCKET_ERROR) { closesocket(Client); return false; }
		tmp = Header + "\r\n";
		tmp += RequestData;
		//申请临时存储内存;
		char *TmpBuf = nullptr;
		try { TmpBuf = new char[1025]; }
		catch (...) { closesocket(Client); return false; }
		std::string tmpret;
		//如果是https使用SSL提交;
#pragma region Https请求提交
		if (IsHttps) {
			//添加SSL错误信息;
			SSL_load_error_strings();
			//添加SSL加密/HASH算法;
			SSLeay_add_ssl_algorithms();
			//客户端，服务端选择SSLv23_server_method() 
			const SSL_METHOD *meth = SSLv23_client_method();
			//建立新的SSL上下文 
			SSL_CTX* ctx = SSL_CTX_new(meth);
			if (ctx == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			int ret;
			SSL*ssl = SSL_new(ctx);
			if (ssl == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			//将SSL与TCP SOCKET 连接 
			SSL_set_fd(ssl, Client);
			//SSL连接 
			ret = SSL_connect(ssl);
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			ret = SSL_write(ssl, tmp.c_str(), tmp.size());
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			do {
				ZeroMemory(TmpBuf, 1025);
				ret = SSL_read(ssl, TmpBuf, 1024);
				tmpret.append(TmpBuf);
			} while (ret);
			//返回数据处理
			HttpCookies = GetCookies(tmpret);
			ReturnCookies.append(HttpCookies);
			ResultData.append(GetHttpData(tmpret));

		}
		else {
			if (send(Client, tmp.c_str(), tmp.size(), 0) == SOCKET_ERROR) { closesocket(Client); delete[]TmpBuf; return false; }
			UINT len = 0;
			do
			{
				ZeroMemory(TmpBuf, 1025);
				len = recv(Client, TmpBuf, 1024, 0);
				tmpret.append(TmpBuf);
			} while (len);
			HttpCookies = GetCookies(tmpret);
			ReturnCookies.append(HttpCookies);
			ResultData.append(GetHttpData(tmpret));
		}
#pragma endregion
		delete[]TmpBuf;
		closesocket(Client);
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	bool _fastcall http2::POST(std::wstring url, std::string &ResultData, std::wstring RequestData, std::wstring Header, std::wstring Cookies, std::wstring ReturnCookies)
	{
		if (!m_IsInitWSA)return false;
		bool IsHttps;
		url.find(L"https") == std::wstring::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		std::wstring tmp = L"POST /" + GetURLPage(url) + L" HTTP/1.0\r\n";
		Header = tmp + Header;
		if (Header.find(L"User-Agent:") == std::string::npos)Header.append(L"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find(L"Accept:") == std::string::npos)Header.append(L"Accept: */*\r\n");
		if (Header.find(L"Accept-Language:") == std::string::npos)Header.append(L"Accept-Language: zh-cn\r\n");
		if (Header.find(L"Content-Type:") == std::string::npos)Header.append(L"Content-Type: application/x-www-form-urlencoded\r\n");
		if (Header.find(L"Host:") == std::string::npos) {
			Header.append(L"Host: ");
			Header.append(GetHost(url));
			Header.append(L"\r\n");
		}
		if (Header.find(L"Referer:") == std::string::npos) {
			Header.append(L"Referer: ");
			Header.append(url);
			Header.append(L"\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find(L"Cookies:") == std::string::npos) {
				Header.append(L"Cookies: ");
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
		}
		wchar_t *Datalen = new wchar_t[256];
		wsprintfW(Datalen, L"%d", RequestData.size());
		Header.append(L"Content-Length:");
		Header.append(Datalen);
		Header.append(L"\r\n");
#pragma endregion
		SOCKADDR_IN Sockaddr = { 0 };
		IsHttps ? Sockaddr.sin_port = htons(443) : Sockaddr.sin_port = htons(80);
		Sockaddr.sin_family = AF_INET;
		//UNICODE转ANSI,因为gethostbyname不支持UNICODE
		char *Tmpr = nullptr;
		try { Tmpr = new char[4096]; }
		catch (...) { return false; }
		ZeroMemory(Tmpr, 4096);
		WideCharToMultiByte(CP_ACP, NULL, url.c_str(), url.length(), Tmpr, 4096, NULL, NULL);
		hostent *hostip = gethostbyname(GetHost(Tmpr).c_str());
		delete[]Tmpr;
		if (!hostip)return false;
		Sockaddr.sin_addr = *reinterpret_cast<in_addr*>(hostip->h_addr);
		SOCKET Client = socket(AF_INET, SOCK_STREAM, 0);
		if (Client == INVALID_SOCKET)return false;
		if (connect(Client, reinterpret_cast<SOCKADDR*>(&Sockaddr), sizeof(Sockaddr)) == SOCKET_ERROR) { closesocket(Client); return false; }
		tmp = Header + L"\r\n";
		tmp += RequestData;
		//申请临时存储内存;
		char *TmpBuf = nullptr;
		try { TmpBuf = new char[1025]; }
		catch (...) { closesocket(Client); return false; }
		std::string tmpret;
		//如果是https使用SSL提交;
#pragma region Https请求提交
		if (IsHttps) {
			//添加SSL错误信息;
			SSL_load_error_strings();
			//添加SSL加密/HASH算法;
			SSLeay_add_ssl_algorithms();
			//客户端，服务端选择SSLv23_server_method() 
			const SSL_METHOD *meth = SSLv23_client_method();
			//建立新的SSL上下文 
			SSL_CTX* ctx = SSL_CTX_new(meth);
			if (ctx == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			int ret;
			SSL*ssl = SSL_new(ctx);
			if (ssl == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			//将SSL与TCP SOCKET 连接 
			SSL_set_fd(ssl, Client);
			//SSL连接 
			ret = SSL_connect(ssl);
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			//UNICODE转ANSI处理(因为SSL_write不支持UNICODE)
			char *Tmpstr = nullptr;
			UINT sLen = tmp.size() + sizeof(wchar_t);
			try { Tmpstr = new char[tmp.size() + sizeof(wchar_t)]; }
			catch (...) { closesocket(Client); delete[]TmpBuf; return false; }
			ZeroMemory(Tmpstr, sLen);
			WideCharToMultiByte(CP_ACP, NULL, tmp.c_str(), tmp.length(), Tmpstr, sLen, NULL, NULL);
			ret = SSL_write(ssl, Tmpstr, sLen);
			delete[]Tmpstr;
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			do {
				ZeroMemory(TmpBuf, 1025);
				ret = SSL_read(ssl, TmpBuf, 1024);
				tmpret.append(TmpBuf);
			} while (ret);
			HttpCookies = GetCookies(tmpret);
			wchar_t *pWide = nullptr;
			try { pWide = new wchar_t[HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t)]; }
			catch (...) { closesocket(Client); delete[]TmpBuf; return false; }
			ZeroMemory(pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			MultiByteToWideChar(CP_ACP, NULL, HttpCookies.c_str(), HttpCookies.size(), pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			ReturnCookies.append(pWide);
			delete[]pWide;
			ResultData.append(GetHttpData(tmpret));
		}
		else {
			////UNICODE转ANSI处理(因为send不支持UNICODE)
			char *Tmpstr = nullptr;
			UINT sLen = tmp.size() + sizeof(wchar_t);
			try { Tmpstr = new char[tmp.size() + sizeof(wchar_t)]; }
			catch (...) { return false; }
			ZeroMemory(Tmpstr, sLen);
			WideCharToMultiByte(CP_ACP, NULL, tmp.c_str(), tmp.length(), Tmpstr, sLen, NULL, NULL);
			if (send(Client, Tmpstr, sLen, 0) == SOCKET_ERROR) { closesocket(Client); delete[]Tmpstr; delete[]TmpBuf; return false; }
			delete[]Tmpstr;
			UINT len = 0;
			do
			{
				ZeroMemory(TmpBuf, 1025);
				len = recv(Client, TmpBuf, 1024, 0);
				tmpret.append(TmpBuf);
			} while (len);
			HttpCookies = GetCookies(tmpret);
			wchar_t *pWide = nullptr;
			try { pWide = new wchar_t[HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t)]; }
			catch (...) { closesocket(Client); delete[]TmpBuf; return false; }
			ZeroMemory(pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			MultiByteToWideChar(CP_ACP, NULL, HttpCookies.c_str(), HttpCookies.size(), pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			ReturnCookies.append(pWide);
			delete[]pWide;
			ResultData.append(GetHttpData(tmpret));
		}
#pragma endregion
		delete[]TmpBuf;
		closesocket(Client);
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	bool _fastcall http2::GET(std::string url, std::string &ResultData, std::string Header, std::string Cookies, std::string ReturnCookies)
	{
		if (!m_IsInitWSA)return false;
		bool IsHttps;
		//把url转小写(不使用)
		//std::transform(url.begin(), url.end(), url.begin(), ::tolower);
		url.find("https") == std::string::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		std::string tmp = "GET /" + GetURLPage(url) + " HTTP/1.0\r\n";
		Header = tmp + Header;
		if (Header.find("User-Agent:") == std::string::npos)Header.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find("Accept:") == std::string::npos)Header.append("Accept:*/*\r\n");
		if (Header.find("Accept-Language:") == std::string::npos)Header.append("Accept-Language:zh-cn\r\n");
		if (Header.find("Content-Type:") == std::string::npos)Header.append("Content-Type:application/x-www-form-urlencoded\r\n");
		if (Header.find("Cache-Control:") == std::string::npos)Header.append("Cache-Control:no-cache\r\n");
		if (Header.find("Host:") == std::string::npos) {
			Header.append("Host:");
			Header.append(GetHost(url));
			Header.append("\r\n");
		}
		if (Header.find("Referer:") == std::string::npos) {
			Header.append("Referer:");
			Header.append(url);
			Header.append("\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find("Cookies:") == std::string::npos) {
				Header.append("Cookies:");
				Header.append(Cookies);
				Header.append("\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append("\r\n");
			}
		}
#pragma endregion
		SOCKADDR_IN Sockaddr = { 0 };
		IsHttps ? Sockaddr.sin_port = htons(443) : Sockaddr.sin_port = htons(80);
		Sockaddr.sin_family = AF_INET;
		hostent *hostip = gethostbyname(GetHost(url).c_str());
		if (!hostip)return false;
		Sockaddr.sin_addr = *reinterpret_cast<in_addr*>(hostip->h_addr);
		SOCKET Client = socket(AF_INET, SOCK_STREAM, 0);
		if (Client == INVALID_SOCKET)return false;
		if (connect(Client, reinterpret_cast<SOCKADDR*>(&Sockaddr), sizeof(Sockaddr)) == SOCKET_ERROR) { closesocket(Client); return false; }
		tmp = Header + "\r\n";
		//申请临时存储内存;
		char *TmpBuf = nullptr;
		try { TmpBuf = new char[1025]; }
		catch (...) { closesocket(Client); return false; }
		std::string tmpret;
		//如果是https使用SSL提交;
#pragma region Https请求提交
		if (IsHttps) {
			//添加SSL错误信息;
			SSL_load_error_strings();
			//添加SSL加密/HASH算法;
			SSLeay_add_ssl_algorithms();
			//客户端，服务端选择SSLv23_server_method() 
			const SSL_METHOD *meth = SSLv23_client_method();
			//建立新的SSL上下文 
			SSL_CTX* ctx = SSL_CTX_new(meth);
			if (ctx == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			int ret;
			SSL*ssl = SSL_new(ctx);
			if (ssl == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			//将SSL与TCP SOCKET 连接 
			SSL_set_fd(ssl, Client);
			//SSL连接 
			ret = SSL_connect(ssl);
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			ret = SSL_write(ssl, tmp.c_str(), tmp.size());
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			do {
				ZeroMemory(TmpBuf, 1025);
				ret = SSL_read(ssl, TmpBuf, 1024);
				tmpret.append(TmpBuf);
			} while (ret);
			HttpCookies = GetCookies(tmpret);
			ReturnCookies.append(HttpCookies);
			ResultData.append(GetHttpData(tmpret));
		}
		else {
			if (send(Client, tmp.c_str(), tmp.size(), 0) == SOCKET_ERROR) { closesocket(Client); delete[]TmpBuf; return false; }
			UINT len = 0;
			do
			{
				ZeroMemory(TmpBuf, 1025);
				len = recv(Client, TmpBuf, 1024, 0);
				tmpret.append(TmpBuf);
			} while (len);
			HttpCookies = GetCookies(tmpret);
			ReturnCookies.append(HttpCookies);
			ResultData.append(GetHttpData(tmpret));
		}
#pragma endregion
		delete[]TmpBuf;
		closesocket(Client);
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	bool _fastcall http2::GET(std::wstring url, std::string &ResultData, std::wstring Header, std::wstring Cookies, std::wstring ReturnCookies)
	{
		if (!m_IsInitWSA)return false;
		bool IsHttps;
		url.find(L"https") == std::wstring::npos ? IsHttps = false : IsHttps = true;
#pragma region 协议头处理
		std::wstring tmp = L"GET /" + GetURLPage(url) + L" HTTP/1.0\r\n";
		Header = tmp + Header;
		if (Header.find(L"User-Agent:") == std::string::npos)Header.append(L"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299\r\n");
		if (Header.find(L"Accept:") == std::string::npos)Header.append(L"Accept: */*\r\n");
		if (Header.find(L"Accept-Language:") == std::string::npos)Header.append(L"Accept-Language: zh-cn\r\n");
		if (Header.find(L"Content-Type:") == std::string::npos)Header.append(L"Content-Type: application/x-www-form-urlencoded\r\n");
		if (Header.find(L"Host:") == std::string::npos) {
			Header.append(L"Host: ");
			Header.append(GetHost(url));
			Header.append(L"\r\n");
		}
		if (Header.find(L"Referer:") == std::string::npos) {
			Header.append(L"Referer: ");
			Header.append(url);
			Header.append(L"\r\n");
		}
		if (!Cookies.empty()) {
			if (Header.find(L"Cookies:") == std::string::npos) {
				Header.append(L"Cookies: ");
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
			else {
				Header.append(Cookies);
				Header.append(L"\r\n");
			}
		}
#pragma endregion
		SOCKADDR_IN Sockaddr = { 0 };
		IsHttps ? Sockaddr.sin_port = htons(443) : Sockaddr.sin_port = htons(80);
		Sockaddr.sin_family = AF_INET;
		//UNICODE转ANSI,因为gethostbyname不支持UNICODE
		char *Tmpr = nullptr;
		try { Tmpr = new char[4096]; }
		catch (...) { return false; }
		ZeroMemory(Tmpr, 4096);
		WideCharToMultiByte(CP_ACP, NULL, url.c_str(), url.length(), Tmpr, 4096, NULL, NULL);
		hostent *hostip = gethostbyname(GetHost(Tmpr).c_str());
		delete[]Tmpr;
		if (!hostip)return false;
		Sockaddr.sin_addr = *reinterpret_cast<in_addr*>(hostip->h_addr);
		SOCKET Client = socket(AF_INET, SOCK_STREAM, 0);
		if (Client == INVALID_SOCKET)return false;
		if (connect(Client, reinterpret_cast<SOCKADDR*>(&Sockaddr), sizeof(Sockaddr)) == SOCKET_ERROR) { closesocket(Client); return false; }
		tmp = Header + L"\r\n";
		//申请临时存储内存;
		char *TmpBuf = nullptr;
		try { TmpBuf = new char[1025]; }
		catch (...) { closesocket(Client); return false; }
		std::string tmpret;
		//如果是https使用SSL提交;
#pragma region Https请求提交
		if (IsHttps) {
			//添加SSL错误信息;
			SSL_load_error_strings();
			//添加SSL加密/HASH算法;
			SSLeay_add_ssl_algorithms();
			//客户端，服务端选择SSLv23_server_method() 
			const SSL_METHOD *meth = SSLv23_client_method();
			//建立新的SSL上下文 
			SSL_CTX* ctx = SSL_CTX_new(meth);
			if (ctx == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			int ret;
			SSL*ssl = SSL_new(ctx);
			if (ssl == NULL) { closesocket(Client); delete[]TmpBuf; return false; }
			//将SSL与TCP SOCKET 连接 
			SSL_set_fd(ssl, Client);
			//SSL连接 
			ret = SSL_connect(ssl);
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			//UNICODE转ANSI处理(因为SSL_write不支持UNICODE)
			char *Tmpstr = nullptr;
			UINT sLen = tmp.size() + sizeof(wchar_t);
			try { Tmpstr = new char[tmp.size() + sizeof(wchar_t)]; }
			catch (...) { closesocket(Client); delete[]TmpBuf; return false; }
			ZeroMemory(Tmpstr, sLen);
			WideCharToMultiByte(CP_ACP, NULL, tmp.c_str(), tmp.length(), Tmpstr, sLen, NULL, NULL);
			ret = SSL_write(ssl, Tmpstr, sLen);
			delete[]Tmpstr;
			if (ret == -1) { closesocket(Client); delete[]TmpBuf; return false; }
			do {
				ZeroMemory(TmpBuf, 1025);
				ret = SSL_read(ssl, TmpBuf, 1024);
				tmpret.append(TmpBuf);
			} while (ret);
			HttpCookies = GetCookies(tmpret);
			wchar_t *pWide = nullptr;
			try { pWide = new wchar_t[HttpCookies.size() * sizeof(wchar_t)+ sizeof(wchar_t)]; }
			catch (...) { closesocket(Client); delete[]TmpBuf; return false; }
			ZeroMemory(pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			MultiByteToWideChar(CP_ACP, NULL, HttpCookies.c_str(), HttpCookies.size(), pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			ReturnCookies.append(pWide);
			delete[]pWide;
			ResultData.append(GetHttpData(tmpret));
		}
		else {
			//UNICODE转ANSI处理(因为Send不支持UNICODE)
			char *Tmpstr = nullptr;
			UINT sLen = tmp.size() + sizeof(wchar_t);
			try { Tmpstr = new char[tmp.size() + sizeof(wchar_t)]; }
			catch (...) { return false; }
			ZeroMemory(Tmpstr, sLen);
			WideCharToMultiByte(CP_ACP, NULL, tmp.c_str(), tmp.length(), Tmpstr, sLen, NULL, NULL);
			if (send(Client, Tmpstr, sLen, 0) == SOCKET_ERROR) { closesocket(Client); delete[]Tmpstr; delete[]TmpBuf; return false; }
			delete[]Tmpstr;
			UINT len = 0;
			do
			{
				ZeroMemory(TmpBuf, 1025);
				len = recv(Client, TmpBuf, 1024, 0);
				tmpret.append(TmpBuf);
			} while (len);
			HttpCookies = GetCookies(tmpret);
			wchar_t *pWide = nullptr;
			try { pWide = new wchar_t[HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t)]; }
			catch (...) { closesocket(Client); delete[]TmpBuf; return false; }
			ZeroMemory(pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			MultiByteToWideChar(CP_ACP, NULL, HttpCookies.c_str(), HttpCookies.size(), pWide, HttpCookies.size() * sizeof(wchar_t) + sizeof(wchar_t));
			ReturnCookies.append(pWide);
			delete[]pWide;
			ResultData.append(GetHttpData(tmpret));
		}
#pragma endregion
		delete[]TmpBuf;
		closesocket(Client);
		bool Resultbool;
		ResultData.length() > 0 ? Resultbool = true : Resultbool = false;
		return Resultbool;
	}
	std::string _fastcall http2::GetLocalCookies(std::string host)
	{
		char *TmpCookies = nullptr;
		DWORD CookiesLen = 4097;
		try { TmpCookies = new char[CookiesLen]; }
		catch (...) { return ""; }
		ZeroMemory(TmpCookies, 4097);
		if (!InternetGetCookieA(host.c_str(), NULL, reinterpret_cast<LPSTR>(TmpCookies), &CookiesLen))return "";
		std::string Cookies(reinterpret_cast<LPSTR>(TmpCookies));
		return Cookies;
	}
	std::wstring _fastcall http2::GetLocalCookies(std::wstring host)
	{
		char *TmpCookies = nullptr;
		DWORD CookiesLen = 4097;
		try { TmpCookies = new char[CookiesLen]; }
		catch (...) { return L""; }
		ZeroMemory(TmpCookies, 4097);
		if (!InternetGetCookieW(host.c_str(), NULL, reinterpret_cast<LPWSTR>(TmpCookies), &CookiesLen))return L"";
		std::wstring Cookies(reinterpret_cast<LPWSTR>(TmpCookies));
		return Cookies;
	}
	VARIANT _fastcall http2::ExecuteJScript(std::string JScript, std::string FuncetionName, std::string ErrorLog)
	{
		std::stringstream loger;
		VARIANT Result = { 0 };
		try {
			m_pScriptControlPtr->Language = "JScript";
			m_pScriptControlPtr->ExecuteStatement(JScript.c_str());
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << "JScript加载错误!\r\n";
			loger << "错误代码: [" << ErrorCode << "]\r\n";
			loger << "出错位置: [" << ErrorLine << "]行\r\n";
			loger << "提示信息: " << e.Description() << "\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str("");
			return Result;
		}
		try {
			Result = m_pScriptControlPtr->Eval(FuncetionName.c_str());
			m_pScriptControlPtr->Reset();
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << "JScript运行错误!\r\n";
			loger << "错误代码: [" << ErrorCode << "]\r\n";
			loger << "出错位置: [" << ErrorLine << "]行\r\n";
			loger << "提示信息: " << e.Description() << "\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str("");
			return Result;
		}
		loger.clear();
		loger.str("");
		return Result;
	}
	VARIANT _fastcall http2::ExecuteJScript(std::wstring JScript, std::wstring FuncetionName, std::wstring ErrorLog)
	{
		std::wstringstream loger;
		VARIANT Result = { 0 };
		try {
			m_pScriptControlPtr->Language = "JScript";
			m_pScriptControlPtr->ExecuteStatement(JScript.c_str());
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << L"JScript加载错误!\r\n";
			loger << L"错误代码: [" << ErrorCode << L"]\r\n";
			loger << L"出错位置: [" << ErrorLine << L"]行\r\n";
			loger << L"提示信息: " << e.Description() << L"\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str(L"");
			return Result;
		}
		try {
			Result = m_pScriptControlPtr->Eval(FuncetionName.c_str());
			m_pScriptControlPtr->Reset();
		}
		catch (_com_error &e) {
			MSScriptControl::IScriptErrorPtr Error = m_pScriptControlPtr->Error;
			long ErrorLine, ErrorCode;
			BSTR ErrorTip;
			Error->get_Number(&ErrorCode);
			Error->get_Line(&ErrorLine);
			Error->get_Description(&ErrorTip);
			loger << L"JScript运行错误!\r\n";
			loger << L"错误代码: [" << ErrorCode << L"]\r\n";
			loger << L"出错位置: [" << ErrorLine << L"]行\r\n";
			loger << L"提示信息: " << e.Description() << L"\r\n";
			ErrorLog = loger.str();
			loger.clear();
			loger.str(L"");
			return Result;
		}
		loger.clear();
		loger.str(L"");
		return Result;
	}
}

