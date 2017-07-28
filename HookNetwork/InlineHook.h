
#ifndef AFX_INLINEHOOK_H__F6C15E16_90FE_42E0_B60A_0D4F125206BD__INCLUDED_
#define AFX_INLINEHOOK_H__F6C15E16_90FE_42E0_B60A_0D4F125206BD__INCLUDED_

#pragma once
#include <WINSOCK2.H>
#pragma comment(lib,"WS2_32.lib")
#include <MSWSOCK.H>
#pragma comment(lib,"Mswsock.lib")

#include <WINDOWS.H>
#include "GlobalStruct.h"

class CInlineHook  
{
public:
	CInlineHook();
	virtual ~CInlineHook();
	//加载 kernel object
	BOOL LoadEventAndMapFile();
	//初始化HOOK点
	BOOL InitializeHookPoint();
	//Inline HOOK
	VOID InlineHook(HOOKADDRINFO *lpHookInfo,BOOL bIsHook = TRUE);
	//开始HOOK
	BOOL StartHook();
	//停止HOOK
	VOID StopHook();
	//我们自己的过滤函数
	static int  WINAPI Hooksendto(SOCKET s,const char FAR *buf,int len,int flags,const struct sockaddr FAR *to,int tolen);
	static BOOL WINAPI HookAcceptEx(SOCKET sListenSocket,SOCKET sAcceptSocket,PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,    DWORD dwRemoteAddressLength,LPDWORD lpdwBytesReceived,LPOVERLAPPED lpOverlapped  );
	static int  WINAPI HookConnect(SOCKET s, const struct sockaddr *name, int namelen);
	static int  WINAPI Hooksend(SOCKET s,const char FAR *buf,int len,int flags);
	static int  WINAPI HookWSARecvFrom(SOCKET s,LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,struct sockaddr FAR *lpFrom,LPINT lpFromlen,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	static int  WINAPI Hookgetpeername(SOCKET s,struct sockaddr FAR *name,int FAR *namelen);
	static int  WINAPI Hookgetsockname(SOCKET s,struct sockaddr FAR *name,int FAR *namelen);
	static int  WINAPI HookWSARecv(SOCKET s, LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	static VOID WINAPI HookGetAcceptExSockaddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, sockaddr **LocalSockaddr, LPINT LocalSockaddrLength, sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength);
public:
	//发送连接房间的数据包
	static BOOL WINAPI SendConnectHousePacket(SOCKET ConSock,in_addr VirtualAddr);
};

#endif 
