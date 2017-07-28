#include "InlineHook.h"
#include <iostream>
using namespace std;

#define NAKED __declspec(naked)
//函数地址结构
HOOKADDRINFO ConnectAddr;
HOOKADDRINFO SendAddr;
HOOKADDRINFO SendtoAddr;
HOOKADDRINFO WSARecvFromAddr;
HOOKADDRINFO AcceptExAddr;
HOOKADDRINFO GetPeernameAddr;
HOOKADDRINFO GetsocknameAddr;
HOOKADDRINFO WSARecvAddr;
HOOKADDRINFO GetAcceptExSockaddrsAddr;
//保存的socket
SOCKET udpsocket;
//内核对象
HANDLE hQueryEvent;
HANDLE hContinueEvent;
HANDLE hMap;
ASSOCIATE_VIRTUAL_IP_ADDR * pavia;
//分配的虚拟地址
in_addr LocalVirtualIpAddr;
//主机虚拟地址
in_addr RemoteVirtualIpAddr;

//test
extern FILE *fp;

CInlineHook::CInlineHook()
{
	hQueryEvent = NULL;
	hContinueEvent = NULL;
	hMap = NULL;
	pavia = NULL;
	udpsocket = NULL;
	ZeroMemory(&ConnectAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&SendtoAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&AcceptExAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&SendAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&WSARecvFromAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&GetPeernameAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&GetsocknameAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&WSARecvAddr,sizeof(HOOKADDRINFO));
	ZeroMemory(&GetAcceptExSockaddrsAddr,sizeof(HOOKADDRINFO));
	//加载内核对象
	LoadEventAndMapFile();
	//初始化HOOK点
	InitializeHookPoint();
	//开始HOOK 网络API
	StartHook();

	fprintf(fp,"Initial Inline Hook Successful\n");
}

CInlineHook::~CInlineHook()
{
	//取消HOOK
	StopHook();
}

//----------------------二次形成新栈帧函数------------------------------------------------
NAKED int WINAPI ConnectMiddle(SOCKET s, const struct sockaddr *name, int namelen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[ConnectAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI SendtoMiddle(SOCKET s, const char *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[SendtoAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED SOCKET WINAPI AcceptExMiddle(SOCKET sListenSocket,SOCKET sAcceptSocket,PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,DWORD dwRemoteAddressLength,LPDWORD lpdwBytesReceived,    LPOVERLAPPED lpOverlapped  )
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[AcceptExAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI sendMiddle(SOCKET s,const char FAR *buf,int len,int flags)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[SendAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}	
}

NAKED int WINAPI WSARecvFromMiddle(SOCKET s,LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,struct sockaddr FAR *lpFrom,LPINT lpFromlen,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[WSARecvFromAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}		
}

NAKED int WINAPI getpeernameMiddle(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[GetPeernameAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI getsocknameMiddle(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[GetsocknameAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED int WINAPI WSARecvMiddle(SOCKET s, LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[WSARecvAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

NAKED VOID WINAPI GetAcceptExSockaddrsMiddle(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, struct sockaddr **LocalSockaddr, LPINT LocalSockaddrLength, struct sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength)
{
	__asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		mov eax,[GetAcceptExSockaddrsAddr.OldFuncAddr];
		add eax,5;
		jmp eax;
	}
}

//--------------------------------------------------------------------------以上全部都是

BOOL CInlineHook::LoadEventAndMapFile()
{
	hQueryEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,MY_QUERY_EVENT_NAME);
	hContinueEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,MY_CONTINUE_ENENT_NAME);
	hMap = OpenFileMapping(FILE_MAP_READ,FALSE,MY_MAPFILE_NAME);
	pavia = (ASSOCIATE_VIRTUAL_IP_ADDR *)MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
	return TRUE;
}

//初始化HOOK点
BOOL CInlineHook::InitializeHookPoint()
{
	ConnectAddr.OldFuncAddr = (DWORD)connect;
	ConnectAddr.NewFuncAddr = (DWORD)HookConnect;
	
	SendtoAddr.OldFuncAddr = (DWORD)sendto;
	SendtoAddr.NewFuncAddr = (DWORD)Hooksendto;

	AcceptExAddr.OldFuncAddr = (DWORD)*(PULONG) *(ULONG**)((ULONG)AcceptEx + 2);
	AcceptExAddr.NewFuncAddr = (DWORD)HookAcceptEx;

	SendAddr.OldFuncAddr = (DWORD)send;
	SendAddr.NewFuncAddr = (DWORD)Hooksend;

	WSARecvFromAddr.OldFuncAddr = (DWORD)WSARecvFrom;
	WSARecvFromAddr.NewFuncAddr = (DWORD)HookWSARecvFrom;

	//GetPeernameAddr
	GetPeernameAddr.OldFuncAddr = (DWORD)getpeername;
	GetPeernameAddr.NewFuncAddr = (DWORD)Hookgetpeername;

	GetsocknameAddr.OldFuncAddr = (DWORD)getsockname;
	GetsocknameAddr.NewFuncAddr = (DWORD)Hookgetsockname;

	WSARecvAddr.OldFuncAddr = (DWORD)WSARecv;
	WSARecvAddr.NewFuncAddr = (DWORD)HookWSARecv;

	GetAcceptExSockaddrsAddr.OldFuncAddr = (DWORD)*(PULONG) *(ULONG**)((ULONG)GetAcceptExSockaddrs + 2);
	GetAcceptExSockaddrsAddr.NewFuncAddr = (DWORD)HookGetAcceptExSockaddrs;
	return TRUE;
}

//Inline HOOK
//参数1:HOOK函数的地址信息 参数2:是否是HOOK 还是UNHOOK
VOID CInlineHook::InlineHook(HOOKADDRINFO * lpHookInfo,BOOL bIsHook)
{
	//原始内存页属性
	DWORD	OrigProtect = 0;

	//HOOK指令 常规的jmp 跳转
	UCHAR   uszHookCode[5] = {0xE9};

	//原始指令<通过分析发现所有的HOOK函数头部都是这样，我们直接头部HOOK>
	UCHAR uszOrigCode[5] = {0x8B,0xFF,0x55,0x8B,0xEC};
	//首先必须要满足两个值都是有效的(无效程序会奔溃掉的)
	if (lpHookInfo->OldFuncAddr && lpHookInfo->NewFuncAddr)
	{
		//修改内存页属性
		VirtualProtect((LPVOID)lpHookInfo->OldFuncAddr,5,PAGE_READWRITE,&OrigProtect);
		if (bIsHook)
		{
			//这里是开始HOOK

			//计算HOOK点  公式:跳转的目标地址 - HOOK 函数的原始地址 - 5 == 原始函数地址的下一条指令的地址
			(*(ULONG*)&uszHookCode[1]) = lpHookInfo->NewFuncAddr - lpHookInfo->OldFuncAddr - 5;
			
			CopyMemory((LPVOID)lpHookInfo->OldFuncAddr,uszHookCode,5);
		}
		else
		{
			//这里是恢复HOOK
			//恢复一下原来的就可以了<这里可能会不稳定，因为恢复的时候一定是在程序运行的时候>
			CopyMemory((LPVOID)lpHookInfo->OldFuncAddr,uszOrigCode,5);
		}
		lpHookInfo->bIsHook = bIsHook;
		//恢复内存页属性
		VirtualProtect((LPVOID)lpHookInfo->OldFuncAddr,5,OrigProtect,&OrigProtect);

	}
}

//开始HOOK
BOOL CInlineHook::StartHook()
{
	InlineHook(&ConnectAddr);
	InlineHook(&SendtoAddr);
	InlineHook(&AcceptExAddr);
	InlineHook(&SendAddr);
	InlineHook(&WSARecvFromAddr);
	InlineHook(&GetPeernameAddr);
	InlineHook(&GetsocknameAddr);
	InlineHook(&WSARecvAddr);
	InlineHook(&GetAcceptExSockaddrsAddr);

	return TRUE;
}

//停止HOOK
VOID CInlineHook::StopHook()
{
	InlineHook(&ConnectAddr,FALSE);
	InlineHook(&SendtoAddr,FALSE);
	InlineHook(&AcceptExAddr,FALSE);
	InlineHook(&SendAddr,FALSE);
	InlineHook(&WSARecvFromAddr,FALSE);
	InlineHook(&GetPeernameAddr,FALSE);
	InlineHook(&GetsocknameAddr,FALSE);
	InlineHook(&WSARecvAddr,FALSE);
	InlineHook(&GetAcceptExSockaddrsAddr,FALSE);
}

//我们自己的过滤函数
int  WINAPI CInlineHook::Hooksendto(SOCKET s,const char FAR *buf,int len,int flags,const struct sockaddr FAR *to,int tolen)
{
	//保存下game udp socket
	udpsocket = s;
	//修改为对战平台
	sockaddr_in localaddr = {0};
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	localaddr.sin_port = htons(CLIENT_PORT);
	fprintf(fp,"sendto local client\n");

	return SendtoMiddle(s,buf,len,flags,(PSOCKADDR)&localaddr,tolen);
}

BOOL WINAPI CInlineHook::HookAcceptEx(SOCKET sListenSocket,SOCKET sAcceptSocket,PVOID lpOutputBuffer,DWORD dwReceiveDataLength,DWORD dwLocalAddressLength,    DWORD dwRemoteAddressLength,LPDWORD lpdwBytesReceived,LPOVERLAPPED lpOverlapped  )
{
	fprintf(fp,"HookAcceptEx success\n");
	return AcceptExMiddle(sListenSocket,sAcceptSocket,lpOutputBuffer,dwReceiveDataLength,dwLocalAddressLength,dwRemoteAddressLength,lpdwBytesReceived,lpOverlapped);
}

int  WINAPI CInlineHook::HookConnect(SOCKET s, const struct sockaddr *name, int namelen)
{
	//通知客户端要连接哪个IP（地图）
	SendConnectHousePacket(udpsocket,((PSOCKADDR_IN)name)->sin_addr);
	//保存主机虚拟地址
	RemoteVirtualIpAddr.S_un.S_addr = ((PSOCKADDR_IN)name)->sin_addr.S_un.S_addr;
	Sleep(100);
	//修改要connect的IP地址
	sockaddr_in VirtualIp;
	VirtualIp.sin_family = AF_INET;
	VirtualIp.sin_port = htons(CLIENT_TCP_GAME);
	VirtualIp.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	fprintf(fp,"HookConnect success\n");
	return ConnectMiddle(s,(PSOCKADDR)&VirtualIp,namelen);
}

int WINAPI CInlineHook::Hooksend(SOCKET s,const char FAR *buf,int len,int flags)
{
	fprintf(fp,"Hooksend success\n");
	return sendMiddle(s,buf,len,flags);
}

int WINAPI CInlineHook::HookWSARecvFrom(SOCKET s,LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,struct sockaddr FAR *lpFrom,LPINT lpFromlen,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine  )
{
	int res;
	//保存原始的缓冲区
	WSABUF wb;
	wb = *lpBuffers;
	//更改缓冲区为自定义的
	UCHAR tempszbuf[MAX_MAP_SIZE] = {0};
	lpBuffers->buf = (PCHAR)tempszbuf;
	//接收
	res = WSARecvFromMiddle(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpFrom,lpFromlen,lpOverlapped,lpCompletionRoutine);
	if(res == 0 && WSAGetLastError() == 0)
	{
		BYTE *SignTrue = (BYTE *)tempszbuf;
		if(*SignTrue == 0xF7)
		{
			CopyMemory(wb.buf,tempszbuf,*lpNumberOfBytesRecvd);
			fprintf(fp,"query packet recv success\n");
		}
		else
		{
			TRANSMIT_PROTOCOL_METHOD *tpm = (TRANSMIT_PROTOCOL_METHOD *)tempszbuf;
			if(tpm->Protocol == TRANSMIT_PROTOCOL_HOUSEMAPINFO)
			{
				TRANSMIT_GAMEHOUSE_MAPINFO *tgm = (TRANSMIT_GAMEHOUSE_MAPINFO *)tempszbuf;
				//地图数据存到缓冲区中
				CopyMemory(wb.buf,&tgm->lpszMapInfo,tgm->uMapLen);
				//修正返回字节的大小
				*lpNumberOfBytesRecvd = tgm->uMapLen;
				//伪造发包源地址
				((PSOCKADDR_IN)lpFrom)->sin_addr.s_addr = tgm->HostVirtualIp.S_un.S_addr;
				fprintf(fp,"map info recv success\n");
			}
			else
			{
				//虚拟ip包不用给游戏 直接return
				TRANSMIT_VIRTUAL_IP *tvi = (TRANSMIT_VIRTUAL_IP *)tempszbuf;
				LocalVirtualIpAddr.S_un.S_addr = tvi->VirtualIpAddr.S_un.S_addr;
				fprintf(fp,"virtual ip address load success\n");
			}
		}
	}
	//把缓冲区换回去
	lpBuffers->buf = wb.buf;

	return res;
}

int  WINAPI CInlineHook::Hookgetpeername(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	int res;
	//调用原始函数获取值
	res = getpeernameMiddle(s,name,namelen);
	//修改
	((PSOCKADDR_IN)name)->sin_addr.S_un.S_addr = RemoteVirtualIpAddr.S_un.S_addr;
	((PSOCKADDR_IN)name)->sin_port = htons(GAME_PORT);

	fprintf(fp,"Hookgetpeername Remote Ip:%s  Port:%d\n",inet_ntoa(((PSOCKADDR_IN)name)->sin_addr),ntohs(((PSOCKADDR_IN)name)->sin_port));
	return res;
}

int  WINAPI CInlineHook::Hookgetsockname(SOCKET s,struct sockaddr FAR *name,int FAR *namelen)
{
	int res;
	//调用原始函数获取值
	res = getsocknameMiddle(s,name,namelen);
	//修改
	((PSOCKADDR_IN)name)->sin_addr.S_un.S_addr = LocalVirtualIpAddr.S_un.S_addr;

	fprintf(fp,"Hookgetsockname Local Ip:%s  Port:%d\n",inet_ntoa(((PSOCKADDR_IN)name)->sin_addr),ntohs(((PSOCKADDR_IN)name)->sin_port));
	return res;
}

int CInlineHook::HookWSARecv(SOCKET s, LPWSABUF lpBuffers,DWORD dwBufferCount,LPDWORD lpNumberOfBytesRecvd,LPDWORD lpFlags,LPWSAOVERLAPPED lpOverlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	fprintf(fp,"HookWSARecv success\n");
	return WSARecvMiddle(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpOverlapped,lpCompletionRoutine);
}

VOID WINAPI CInlineHook::HookGetAcceptExSockaddrs(PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, sockaddr **LocalSockaddr, LPINT LocalSockaddrLength, sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength)
{
	//调用原始函数获取值
	GetAcceptExSockaddrsMiddle(lpOutputBuffer,dwReceiveDataLength,dwLocalAddressLength,dwRemoteAddressLength,LocalSockaddr,LocalSockaddrLength,RemoteSockaddr,RemoteSockaddrLength);
	//修改
	SetEvent(hQueryEvent);
	if(WAIT_OBJECT_0 == WaitForSingleObject(hContinueEvent,INFINITE))
	{
		//等到同步信息
		(*(sockaddr_in **)LocalSockaddr)->sin_addr.S_un.S_addr = LocalVirtualIpAddr.S_un.S_addr;
		(*(sockaddr_in **)RemoteSockaddr)->sin_addr.S_un.S_addr = pavia->ClientVirtualIp.S_un.S_addr;

		fprintf(fp,"HookGetAcceptExSockAddrs, local ip:%s port:%d\n",inet_ntoa((*(sockaddr_in **)LocalSockaddr)->sin_addr),ntohs((*(sockaddr_in **)LocalSockaddr)->sin_port));
		fprintf(fp,"HookGetAcceptExSockAddrs, remote ip:%s port:%d\n",inet_ntoa((*(sockaddr_in **)RemoteSockaddr)->sin_addr),ntohs((*(sockaddr_in **)RemoteSockaddr)->sin_port));
	}
}

BOOL WINAPI CInlineHook::SendConnectHousePacket(SOCKET ConSock,in_addr VirtualAddr)
{
	TRANSMIT_CONNECT_INFO tci;
	tci.Type = TRANSMIT_PROTOCOL_CONNECTHOUSE;
	tci.HostVirtualIpAddr.S_un.S_addr = VirtualAddr.S_un.S_addr;
	Hooksendto(ConSock,(PCHAR)&tci,sizeof(TRANSMIT_CONNECT_INFO),0,NULL,sizeof(sockaddr_in));
	return TRUE;
}

