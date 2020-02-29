#ifndef __lib_htpp_h
#define __lib_http_h
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <wininet.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <openssl/ssl.h> //openssl支持库.http://slproweb.com/products/Win32OpenSSL.html

#import <msscript.ocx>
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libssl.lib")
#pragma warning(disable:4996)

namespace lib_http
{
	using namespace std;
	using namespace MSScriptControl;
	//下载回调函数,返回true继续下载,返回false停止下载;
	//@parameter 下载文件总大小
	//@parameter 当前下载数据大小
	//@parameter 每秒下载数据大小
	typedef bool(*DownloadProc)(size_t FileSize, size_t DownloadSize, size_t SecondSize);
	class http
	{
	public:
		http();
		~http();
		//POST请求
		//@parameter 请求地址
		//@parameter 返回数据
		//@parameter 请求数据
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall POST(std::string url, std::string &ResultData, std::string RequestData = "", std::string Header = "", std::string Cookies = "", std::string ReturnCookies = "");
		//POST请求
		//@parameter 请求地址
		//@parameter 返回数据(返回string类型,转换交给函数外)
		//@parameter 请求数据
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall POST(std::wstring url, std::string &ResultData, std::wstring RequestData = L"", std::wstring Header = L"", std::wstring Cookies = L"", std::wstring ReturnCookies = L"");
		//GET请求
		//@parameter 请求地址
		//@parameter 返回数据
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall GET(std::string url, std::string &ResultData, std::string Header = "", std::string Cookies = "", std::string ReturnCookies = "");
		//GET请求
		//@parameter 请求地址
		//@parameter 返回数据(返回string类型,转换交给函数外)
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall GET(std::wstring url, std::string &ResultData, std::wstring Header = L"", std::wstring Cookies = L"", std::wstring ReturnCookies = L"");
		//下载文件
		//@parameter 下载地址
		//@parameter 保存文件目录
		//@parameter 回调函数 DownloadProc(size_t FileSize, size_t DownloadSize, size_t SecondSize);
		bool _fastcall DownloadFile(std::string url, std::string FilePath, DownloadProc DownloadPr = nullptr);
		//下载文件
		//@parameter 下载地址
		//@parameter 保存文件目录
		//@parameter 回调函数 DownloadProc(size_t FileSize, size_t DownloadSize, size_t SecondSize);
		bool _fastcall DownloadFile(std::wstring url, std::wstring FilePath, DownloadProc DownloadPr = nullptr);
		//获取指定网址本地IE Cookies
		//@parameter 主机Url地址
		std::string _fastcall GetLocalCookies(std::string host);
		//获取指定网址本地IE Cookies
		//@parameter 主机Url地址
		std::wstring _fastcall GetLocalCookies(std::wstring host);
		//获取网页页面地址
		//@parameter url
		std::string _fastcall GetURLPage(std::string url);
		//获取网页页面地址
		//@parameter url
		std::wstring _fastcall GetURLPage(std::wstring url);
		//获取指定Url的Host地址
		//@parameter Url
		std::string _fastcall GetHost(std::string url);
		//获取指定Url的Host地址
		//@parameter Url
		std::wstring _fastcall GetHost(std::wstring url);
		//运行JScript
		//@parameter JScript代码
		//@parameter JScript函数名及参数func(a,b)
		VARIANT _fastcall ExecuteJScript(std::string JScript, std::string FuncetionName, std::string ErrorLog = "");
		//运行JScript
		//@parameter JScript代码
		//@parameter JScript函数名及参数func(a,b)
		VARIANT _fastcall ExecuteJScript(std::wstring JScript, std::wstring FuncetionName, std::wstring ErrorLog = L"");
		//正则表达式
		//@parameter 正则表达式
		//@parameter 搜索目标字符串
		//@parameter 是否区分大小写
		//@parameter 是否匹配多行
		//@parameter 是否全局匹配
		bool _fastcall RegEx_FindString(std::string Regex, std::string DestString, bool IsCase = false, bool IsMultiRows = true, bool IsGlobal = true);
		//正则表达式
		//@parameter 正则表达式
		//@parameter 搜索目标字符串
		//@parameter 是否区分大小写
		//@parameter 是否匹配多行
		//@parameter 是否全局匹配
		bool _fastcall RegEx_FindString(std::wstring Regex, std::wstring DestString, bool IsCase = false, bool IsMultiRows = true, bool IsGlobal = true);
	private:
		IScriptControlPtr m_pScriptControlPtr = nullptr;

	};
	//Http Socket方式请求;
	class http2
	{
	public:
		http2();
		~http2();
		//POST请求
		//@parameter 请求地址
		//@parameter 返回数据
		//@parameter 请求数据
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall POST(std::string url, std::string &ResultData, std::string RequestData = "", std::string Header = "", std::string Cookies = "", std::string ReturnCookies = "");
		//POST请求
		//@parameter 请求地址
		//@parameter 返回数据(返回string类型,转换交给函数外)
		//@parameter 请求数据
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall POST(std::wstring url, std::string &ResultData, std::wstring RequestData = L"", std::wstring Header = L"", std::wstring Cookies = L"", std::wstring ReturnCookies = L"");
		//GET请求
		//@parameter 请求地址
		//@parameter 返回数据
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall GET(std::string url, std::string &ResultData, std::string Header = "", std::string Cookies = "", std::string ReturnCookies = "");
		//GET请求
		//@parameter 请求地址
		//@parameter 返回数据(返回string类型,转换交给函数外)
		//@parameter 请求协议头
		//@parameter 请求Cookies
		//@parameter 返回Cokies
		bool _fastcall GET(std::wstring url, std::string &ResultData, std::wstring Header = L"", std::wstring Cookies = L"", std::wstring ReturnCookies = L"");
		//获取指定网址本地IE Cookies
		//@parameter 主机Url地址
		std::string _fastcall GetLocalCookies(std::string host);
		//获取指定网址本地IE Cookies
		//@parameter 主机Url地址
		std::wstring _fastcall GetLocalCookies(std::wstring host);
		//获取网页页面地址
		//@parameter url
		std::string _fastcall GetURLPage(std::string url);
		//获取网页页面地址
		//@parameter url
		std::wstring _fastcall GetURLPage(std::wstring url);
		//获取指定Url的Host地址
		//@parameter Url
		std::string _fastcall GetHost(std::string url);
		//获取指定Url的Host地址
		//@parameter Url
		std::wstring _fastcall GetHost(std::wstring url);
		//获取网页信息数据
		//@parameter 服务器返回的网页数据
		std::string _fastcall GetHttpData(std::string urldata);
		//运行JScript
		//@parameter JScript代码
		//@parameter JScript函数名及参数func(a,b)
		VARIANT _fastcall ExecuteJScript(std::string JScript, std::string FuncetionName, std::string ErrorLog = "");
		//运行JScript
		//@parameter JScript代码
		//@parameter JScript函数名及参数func(a,b)
		VARIANT _fastcall ExecuteJScript(std::wstring JScript, std::wstring FuncetionName, std::wstring ErrorLog = L"");
	private:
		IScriptControlPtr m_pScriptControlPtr = nullptr;
		WSADATA m_wsa;
		bool m_IsInitWSA = false;
		string HttpCookies;
	};
}

#endif
