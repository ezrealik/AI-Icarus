#pragma once
#ifndef _LIB_SOCKET_H
#define _LIB_SOCKET_H
#define FD_SETSIZE 1024
#include <windows.h>
#include <iostream>
#include <vector>
#include <process.h>

#pragma comment(lib,"ws2_32.lib")
namespace lib_Socket
{
#pragma region TCP客户端类
	/*
	TCP处理函数指针;
	@parameter 接收Socket
	@parameter Socket索引值
	@parameter 接收缓冲区
	@parameter 接收长度(最大4095)
	@parameter 错误代码
	@parameter 预设参数
	*/
	typedef void(*pTCPEventFunc)(SOCKET nSock, unsigned int nIndex, void *pBuf, unsigned int BufLen, unsigned int ErrorCode, LPARAM lParam);
	//TCP信息数据类型
	struct TCPSOCKET
	{
		SOCKADDR_IN SocketAddr;
		SOCKET Socket;
		unsigned int Port;
		std::string ServerAddr;
		std::wstring ServerAddrW;
		pTCPEventFunc EventFunc;
	};
	//UDP信息数据类型
	struct UDPSOCKET
	{
		SOCKADDR_IN SocketAddr;
		SOCKET Socket;
		unsigned int Port;
		std::string ServerAddr;
		std::wstring ServerAddrW;
		pTCPEventFunc EventFunc;
	};
	//错误信息数据类型
	struct SOCKETERROR
	{
		std::string ErrorInfo;
		std::wstring ErrorInfoW;
		int ErrorCode;
	};
	//TCP全局处理线程;
	static void __stdcall TCPThread();
	class TCPClient
	{
	public:
		TCPClient();
		~TCPClient();
		//TCP初始化
		bool __fastcall Initialize();
		//TCP释放
		void __fastcall Release();
		/*
		创建TCP连接
		@parameter 服务器地址
		@parameter 服务器端口
		*/
		bool __fastcall CreateTCP(std::string ServerAddr, unsigned int Port);
		/*
		创建TCP连接
		@parameter 服务器地址
		@parameter 服务器端口
		*/
		bool __fastcall CreateTCP(std::wstring ServerAddr, unsigned int Port);
		/*
		断开TCP连接
		@parameter TCP连接套接字
		*/
		bool __fastcall DisConnect(SOCKET ClientSocket);
		/*
		查询TCP Socket
		*/
		std::vector<TCPSOCKET> __fastcall QueryTCPSocket();
		/*
		端对端发送TCP数据
		@parameter TCP连接套接字
		@parameter TCP数据
		*/
		void __fastcall SendData(SOCKET ClientSocket, void *pBuf, int BufLen);
		/*
		批量发送TCP数据
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter TCP数据
		*/
		void __fastcall SendData(std::string ServerAddr, unsigned int Port, void *pBuf, int BufLen);
		/*
		批量发送TCP数据
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter TCP数据
		*/
		void __fastcall SendData(std::wstring ServerAddr, unsigned int Port, void *pBuf, int BufLen);
		/*
		设置TCP接收处理函数
		@parameter TCP连接套接字
		@parameter 处理函数
		*/
		void __fastcall SetFunc(SOCKET ClientSocket, pTCPEventFunc EventFunc);
		/*
		设置TCP接收处理函数
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter 处理函数
		*/
		void __fastcall SetFunc(std::string ServerAddr, unsigned int Port, pTCPEventFunc EventFunc);
		/*
		设置TCP接收处理函数
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter 处理函数
		*/
		void __fastcall SetFunc(std::wstring ServerAddr, unsigned int Port, pTCPEventFunc EventFunc);
		/*
		获取错误信息
		*/
		SOCKETERROR __fastcall GetError();
	private:
		
	};
#pragma endregion

#pragma region UDP客户端类
	static void __stdcall UDPThread();
	class UDPClient
	{
	public:
		UDPClient();
		~UDPClient();
		//初始化UDP
		bool __fastcall Initialize();
		//释放UDP
		void __fastcall Release();
		//获取错误信息
		SOCKETERROR __fastcall GetError();
		/*
		创建TCP连接
		@parameter 服务器地址
		@parameter 服务器端口
		*/
		bool __fastcall CreateUDP(std::string ServerAddr, unsigned int Port);
		/*
		创建TCP连接
		@parameter 服务器地址
		@parameter 服务器端口
		*/
		bool __fastcall CreateUDP(std::wstring ServerAddr, unsigned int Port);
		/*
		查询UDP Socket
		*/
		std::vector<UDPSOCKET> __fastcall QueryUDPSocket();
		/*
		端对端发送UDP数据
		@parameter UDP连接套接字
		@parameter UDP数据
		*/
		void __fastcall SendData(SOCKET ClientSocket, void *pBuf, int BufLen);
		/*
		批量发送UDP数据
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter UDP数据
		*/
		void __fastcall SendData(std::string ServerAddr, unsigned int Port, void *pBuf, int BufLen);
		/*
		批量发送UDP数据
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter UDP数据
		*/
		void __fastcall SendData(std::wstring ServerAddr, unsigned int Port, void *pBuf, int BufLen);
		/*
		设置UDP接收处理函数
		@parameter UDP连接套接字
		@parameter 处理函数
		*/
		void __fastcall SetFunc(SOCKET ClientSocket, pTCPEventFunc EventFunc);
		/*
		设置UDP接收处理函数
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter 处理函数
		*/
		void __fastcall SetFunc(std::string ServerAddr, unsigned int Port, pTCPEventFunc EventFunc);
		/*
		设置UDP接收处理函数
		@parameter 服务器地址
		@parameter 服务器端口
		@parameter 处理函数
		*/
		void __fastcall SetFunc(std::wstring ServerAddr, unsigned int Port, pTCPEventFunc EventFunc);
	private:

	};
#pragma endregion
}

#endif