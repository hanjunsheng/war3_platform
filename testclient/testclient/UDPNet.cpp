#include "stdafx.h"
#include "UDPNet.h"

CUDPNet::CUDPNet(void)
{
	this->m_udpserversocket = NULL;
	this->m_udpgamesocket = NULL;
	this->m_comioport = NULL;
	this->m_tcpListensocket = NULL;
	this->IsCreateTcp = FALSE;
	this->IsHostRoom = FALSE;
	this->m_hQueryEvent = NULL;
	this->m_hContinueEvent = NULL;
	this->m_hMap = NULL;
	this->m_pavia = NULL;

	ZeroMemory(&m_VirualHost,sizeof(VIRTUALCLIENT));
	ZeroMemory(&m_VirualHostServer,sizeof(VIRTUALCLIENT));
	m_serveraddr.sin_family = AF_INET;
	m_serveraddr.sin_port = htons(SERVER_PORT);
	m_serveraddr.sin_addr.s_addr = inet_addr("202.118.193.68");
}

CUDPNet::~CUDPNet(void)
{
	list<VIRTUALCLIENT *>::iterator ite = this->m_lstclient.begin();
	while(ite != this->m_lstclient.begin())
	{
		delete *ite;
		*ite = NULL;
		++ite;
	}
}

BOOL CUDPNet::InitialUDP()
{
	//1.加载库
	WORD wVersionRequested;
    WSADATA wsaData;

	wVersionRequested = MAKEWORD(2,2);
	WSAStartup(wVersionRequested,&wsaData);

	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		return false;
	}

	//创建socket
	m_udpserversocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(m_udpserversocket == INVALID_SOCKET)
	{
		UnInitial();
		return FALSE;
	}

	//绑定bind
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(CLIENT_PORT);

	if(SOCKET_ERROR == bind(m_udpserversocket,(const sockaddr *)&addr,sizeof(sockaddr_in)))
	{
		UnInitial();
		return false;
	}

	//设置socket属性
	bool optval = true;
	setsockopt(m_udpserversocket,SOL_SOCKET,SO_BROADCAST,(const char *)&optval,sizeof(bool));

	//获得系统的核数
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	//创建完成端口
	this->m_comioport = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,0);
	if(m_comioport == NULL)
	{
		UnInitial();
		return FALSE;
	}

	//将socket交给完成端口管理
	CreateIoCompletionPort((HANDLE)m_udpserversocket,m_comioport,m_udpserversocket,0);

	//初始化MYSOCKETEX
	PostRecvfrom(m_udpserversocket);

	//创建工作线程
	for(DWORD i=0;i<si.dwNumberOfProcessors * 2;i++)
	{
		HANDLE hthread = (HANDLE)_beginthreadex(NULL,0,&ThreadComIOPortWork,this,0,NULL);
		if(hthread)
		{
			this->m_lstthreadhandle.push_back(hthread);
		}
	}

	//开启游戏UDP
	CreateGameUdpSocket();

	//创建内核对象
	this->m_hQueryEvent = CreateEvent(NULL,FALSE,FALSE,MY_QUERY_EVENT_NAME);
	this->m_hContinueEvent = CreateEvent(NULL,FALSE,FALSE,MY_CONTINUE_ENENT_NAME);
	this->m_hMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE|SEC_COMMIT,0,ONE_PAGE_SZIE,MY_MAPFILE_NAME);
	this->m_pavia = (ASSOCIATE_VIRTUAL_IP_ADDR *)MapViewOfFile(this->m_hMap,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);

	return TRUE;
}

BOOL CUDPNet::CreateGameUdpSocket()
{
	//创建socket
	m_udpgamesocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(m_udpgamesocket == INVALID_SOCKET)
	{
		UnInitial();
		return FALSE;
	}

	//绑定bind
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(CLIENT_PORT);

	if(SOCKET_ERROR == bind(m_udpgamesocket,(const sockaddr *)&addr,sizeof(sockaddr_in)))
	{
		UnInitial();
		return false;
	}

	//设置socket属性
	bool optval = true;
	setsockopt(m_udpgamesocket,SOL_SOCKET,SO_BROADCAST,(const char *)&optval,sizeof(bool));

	//将socket交给完成端口管理
	CreateIoCompletionPort((HANDLE)m_udpgamesocket,m_comioport,m_udpgamesocket,0);

	//初始化MYSOCKETEX
	PostRecvfrom(m_udpgamesocket);

	return TRUE;
}

BOOL CUDPNet::PostRecvfrom(SOCKET sock)
{
	MYOVERINFO *pmyoverinfo = this->m_myoverlapped.NewOverInfoAndAddList();
	ASSERT(pmyoverinfo);
	ZeroMemory(pmyoverinfo,sizeof(MYOVERINFO));
	pmyoverinfo->type = NT_RECVFROM;
	pmyoverinfo->wv.hEvent = WSACreateEvent();
	pmyoverinfo->wb.buf = pmyoverinfo->szbuf;
	pmyoverinfo->wb.len = sizeof(pmyoverinfo->szbuf);
	pmyoverinfo->sockaddrfromlen = sizeof(sockaddr_in)+16;

	DWORD dwNumberOfBytesRecvd;
	DWORD dwFlags = 0;

	if(WSARecvFrom(sock,&pmyoverinfo->wb,1,&dwNumberOfBytesRecvd,&dwFlags,(sockaddr *)&pmyoverinfo->sockaddrfrom,&pmyoverinfo->sockaddrfromlen,&pmyoverinfo->wv,NULL))
	{
		int error = WSAGetLastError();
		if(error != WSA_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

unsigned __stdcall CUDPNet::ThreadComIOPortWork(void * lpvoid)
{
	CUDPNet * pthis = (CUDPNet *)lpvoid;
	BOOL result;
	DWORD dwNumberOfBytes;
	ULONG_PTR CompletionKey;
	MYOVERINFO * pmyoverinfo;

	while(TRUE)
	{
		result = GetQueuedCompletionStatus(pthis->m_comioport,&dwNumberOfBytes,&CompletionKey,(LPOVERLAPPED*)&pmyoverinfo,INFINITE);
		if(result == TRUE)
		{
			if(pmyoverinfo && CompletionKey)
			{
				switch(pmyoverinfo->type)
				{
				case NT_RECVFROM:
					{
						pthis->PostRecvfrom((SOCKET)CompletionKey);
						pthis->DeadUDPData(pmyoverinfo);
						pthis->m_myoverlapped.DelOverInfoFromList(pmyoverinfo);
					}
					break;
				case NT_SENDTO:
					{
						pthis->m_myoverlapped.DelOverInfoFromList(pmyoverinfo);
					}
					break;
				case NT_ACCEPT:
					{
						pthis->m_VirualHost.Assosocket.sock1 = pthis->m_VirualHost.sock;
						CreateIoCompletionPort((HANDLE)pthis->m_VirualHost.sock,pthis->m_comioport,(ULONG_PTR)&pthis->m_VirualHost,0);
						pthis->PostRecv(pthis->m_VirualHost.sock);
						pthis->m_myoverlapped.DelOverGameInfoFromList((MYOVERGAMEINFO *)pmyoverinfo);
					}
					break;
				case NT_RECV:
					{
						pthis->PostRecv(((VIRTUALCLIENT *)CompletionKey)->sock);
						//处理函数
						pthis->DealGamePacket((MYOVERGAMEINFO *)pmyoverinfo,(VIRTUALCLIENT *)CompletionKey,dwNumberOfBytes);
						pthis->m_myoverlapped.DelOverGameInfoFromList((MYOVERGAMEINFO *)pmyoverinfo);
					}
					break;
				case NT_SEND:
					{
						pthis->m_myoverlapped.DelOverGameInfoFromList((MYOVERGAMEINFO *)pmyoverinfo);
					}
					break;
				default:
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			pthis->DealTcpBreakUp();
		}
	}

	return 0;
}

void CUDPNet::DealTcpBreakUp()
{
	if(this->m_VirualHost.sock != 0)
	{
		closesocket(this->m_VirualHost.sock);
		ZeroMemory(&this->m_VirualHost,sizeof(VIRTUALCLIENT));
	}

	if(this->m_VirualHostServer.sock != 0)
	{
		closesocket(this->m_VirualHostServer.sock);
		ZeroMemory(&this->m_VirualHostServer,sizeof(VIRTUALCLIENT));
	}

	//清楚客户端
	list<VIRTUALCLIENT *>::iterator ite = this->m_lstclient.begin();
	while(ite != this->m_lstclient.end())
	{
		closesocket((*ite)->sock);
		delete *ite;
		*ite = NULL;
		++ite;
	}
}

void CUDPNet::DeadUDPData(MYOVERINFO *pmyoverinfo)
{
	BYTE *SignTrue = (BYTE *)pmyoverinfo->szbuf;
	if(*SignTrue == 0xF7)
	{
		DealGameUDPData(pmyoverinfo);
	}
	else
	{
		DealTransmitData(pmyoverinfo);
	}
}

void CUDPNet::DealGameUDPData(MYOVERINFO *pmyoverinfo)
{
	WAR3_UDP_HEAD_PACKET *pinfo = (WAR3_UDP_HEAD_PACKET *)pmyoverinfo->szbuf;
	int type = pinfo->Operation;
	static UCHAR szQueryPacket[0x10] = {0};
	static CHAR szLastRecvBuffer[MAX_PATH] = {0};

	switch(type)
	{
	case UDP_PACKET_QUERY_HOST:
		{
			TRACE("查询局域网内主机\n");
			//发送本地虚拟IP地址
			this->SendToLocalVirualIpAddrToGame();
			//保存查询包
			CopyMemory(szQueryPacket,pmyoverinfo->szbuf,0x10);
			//发送至服务器
			TRACE("发送数据长度:%d \r\n",pinfo->PacketLen);
			this->SendToServerUDPPacket((WAR3_UDP_HEAD_PACKET *)pmyoverinfo->szbuf);
			//清空链表
			list<VIRTUALCLIENT *>::iterator ite = this->m_lstclient.begin();
			while(ite != this->m_lstclient.begin())
			{
				delete *ite;
				*ite = NULL;
				++ite;
			}
			this->m_lstclient.clear();
		}
		break;
	case UDP_PACKET_ECHO_HOST:
		{
			//获取游戏地图包
			TRACE("获取地图\n");
			//发送至服务器
			TRACE("发送数据长度:%d \r\n",pinfo->PacketLen);
			this->SendToServerUDPPacket((WAR3_UDP_HEAD_PACKET *)pmyoverinfo->szbuf);
		}
		break;
	case UDP_PACKET_CREATE_HOST:
		{
			TRACE("创建房间\n");
			//发送至服务器
			TRACE("发送数据长度:%d \r\n",pinfo->PacketLen);
			this->SendToServerUDPPacket((WAR3_UDP_HEAD_PACKET *)pmyoverinfo->szbuf);
			//通知游戏返回地图包信息
			Sleep(20);
			PostSendto(this->m_udpgamesocket,(PCHAR)szQueryPacket,0x10,&pmyoverinfo->sockaddrfrom);
			//修改为主机
			this->IsHostRoom = TRUE;
		}
		break;
	case UDP_PACKET_PEOPLECHANGE:
		{
			TRACE("玩家数量变更\n");
			int packetlength = pinfo->PacketLen;
			if (memcmp(szLastRecvBuffer,pmyoverinfo->szbuf,packetlength) != 0)
			{
				//说明本次和上次封包不相同，我们需要发送到对战平台服务端上面去
				ZeroMemory(szLastRecvBuffer,MAX_PATH);
				CopyMemory(szLastRecvBuffer,pmyoverinfo->szbuf,packetlength);
				TRACE("发送数据长度:%d \r\n",packetlength);
				this->SendToServerUDPPacket((WAR3_UDP_HEAD_PACKET *)pmyoverinfo->szbuf);
			}
		}
		break;
	case UDP_PACKET_CANCELHOST:
		{
			TRACE("主机关闭\n");
			//发送至服务器
			TRACE("发送数据长度:%d \r\n",pinfo->PacketLen);
			this->SendToServerUDPPacket((WAR3_UDP_HEAD_PACKET *)pmyoverinfo->szbuf);
			//修改为客户机
			this->IsHostRoom = FALSE;
		}
		break;
	default:
		TRACE("收到UDP包有误\n");
		break;
	}
}

void CUDPNet::DealTransmitData(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_PROTOCOL_METHOD *method = (TRANSMIT_PROTOCOL_METHOD *)pmyoverinfo->szbuf;

	switch(method->Protocol)
	{
	case TRANSMIT_PROTOCOL_VIRTUALIPADDR:
		{
			TRANSMIT_VIRTUAL_IP *tvi = (TRANSMIT_VIRTUAL_IP *)pmyoverinfo->szbuf;
			this->m_VirtualIP.S_un.S_addr = tvi->VirtualIpAddr.S_un.S_addr;
		}
		break;
	case TRANSMIT_PROTOCOL_HOUSEMAPINFO:
		{
			DealHostMapInfo(pmyoverinfo);
		}
		break;
	case TRANSMIT_PROTOCOL_PEOPLECHANGE:
		{
			DealPeopleChange(pmyoverinfo);
		}
		break;
	case TRANSMIT_PROTOCOL_HOUSECANCEL:
		{
			DealHostCancle(pmyoverinfo);
		}
		break;
	case TRANSMIT_PROTOCOL_CONNECTHOUSE:
		{
			DealConnectHostRoom(pmyoverinfo);
		}
		break;
	case TRANSMIT_PROTOCOL_CLIENTINFO:
		{
			DealClientConnectData(pmyoverinfo);
		}
		break;
	}
}

BOOL CUDPNet::PostSendto(SOCKET sock,char *szbuf,int buflen,sockaddr_in *sendto)
{
	MYOVERINFO *pmyoverinfo = this->m_myoverlapped.NewOverInfoAndAddList();
	ASSERT(pmyoverinfo);
	ZeroMemory(pmyoverinfo,sizeof(MYOVERINFO));
	DWORD dwNumberOfBytesSent;
	DWORD dwFlags = 0;
	
	pmyoverinfo->type = NT_SENDTO;
	pmyoverinfo->wv.hEvent = WSACreateEvent();
	pmyoverinfo->wb.buf = szbuf;
	pmyoverinfo->wb.len = buflen;

	WSASendTo(sock,&pmyoverinfo->wb,1,&dwNumberOfBytesSent,dwFlags,(sockaddr *)sendto,sizeof(sockaddr_in),(LPWSAOVERLAPPED)&pmyoverinfo->wv,NULL);

	return TRUE;
}

void CUDPNet::UnInitial()
{
	//发送完成端口退出消息
	int num = this->m_lstthreadhandle.size();
	while(num)
	{
		PostQueuedCompletionStatus(this->m_comioport,NULL,NULL,NULL);
		num--;
	}

	//关闭线程
	list<HANDLE>::iterator ite = this->m_lstthreadhandle.begin();
	while(ite != this->m_lstthreadhandle.end())
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(*ite,20))
		{
			TerminateThread(*ite,-1);
		}
		CloseHandle(*ite);
		*ite = NULL;
		++ite;
	}

	if(m_udpserversocket)
	{
		closesocket(m_udpserversocket);
		m_udpserversocket = NULL;
	}

	if(m_udpgamesocket)
	{
		closesocket(m_udpgamesocket);
		m_udpgamesocket = NULL;
	}

	WSACleanup();
}

BOOL CUDPNet::SendToServerUDPPacket(WAR3_UDP_HEAD_PACKET * szbuf)
{
	BOOL bIsSend = FALSE;
	TRANSMIT_GAME_UDP_PACKET *tgup = NULL;
	tgup = (TRANSMIT_GAME_UDP_PACKET *)new BYTE[szbuf->PacketLen + sizeof(INT)];
	tgup->Type = TRANSMIT_PROTOCOL_GAMEUDP;
	CopyMemory((PVOID)&tgup->vBody,szbuf,szbuf->PacketLen);
	bIsSend = PostSendto(this->m_udpserversocket,(char *)tgup,szbuf->PacketLen + sizeof(INT),&this->m_serveraddr);
	delete[] tgup;
	return bIsSend;
}

BOOL CUDPNet::SendToUserLoginInfo(PCHAR szName,PCHAR szPassword)
{
	BOOL bIsSend = FALSE;
	TRANSMIT_USER_LOGIN tul;
	tul.Type = TRANSMIT_PROTOCOL_USERLOGIN;
	CopyMemory(tul.szUserName,szName,MAX_USER_NAME);
	CopyMemory(tul.szUserPassword,szPassword,MAX_USER_NAME);
	bIsSend = PostSendto(this->m_udpserversocket,(char *)&tul,sizeof(TRANSMIT_USER_LOGIN),&this->m_serveraddr);
	return bIsSend;
}

BOOL CUDPNet::SendToUserQuitInfo()
{
	BOOL bIsSend = FALSE;
	TRANSMIT_USER_QUIT tuq;
	tuq.Type = TRANSMIT_PROTOCOL_USERQUIT;
	tuq.VirtualIPAddr.S_un.S_addr = this->m_VirtualIP.S_un.S_addr;
	bIsSend = PostSendto(this->m_udpserversocket,(char *)&tuq,sizeof(TRANSMIT_USER_QUIT),&this->m_serveraddr);
	return bIsSend;
}

void CUDPNet::DealHostMapInfo(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_GAMEHOUSE_MAPINFO *tgm = (TRANSMIT_GAMEHOUSE_MAPINFO *)pmyoverinfo->szbuf;
	SendToGameUdp((PCHAR)tgm,sizeof(TRANSMIT_GAMEHOUSE_MAPINFO) - 4 + tgm->uMapLen);
}

BOOL CUDPNet::SendToGameUdp(PCHAR szbuf,UINT ubuflen)
{
	sockaddr_in GameAddr = {0};
	//采取游戏默认端口号
	GameAddr.sin_family = AF_INET;
	GameAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	GameAddr.sin_port = htons(GAME_PORT);
	//发送给游戏
	PostSendto(m_udpgamesocket,szbuf,ubuflen,&GameAddr);
	return TRUE;
}

void CUDPNet::DealPeopleChange(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_GAMEHOUSE_PEOPLECHANGE *tgp = (TRANSMIT_GAMEHOUSE_PEOPLECHANGE *)pmyoverinfo->szbuf;
	SendToGameUdp((PCHAR)&tgp->War3ChangePeople,sizeof(TRANSMIT_GAMEHOUSE_PEOPLECHANGE));
}

void CUDPNet::DealHostCancle(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_GAMEHOUSE_CANCLE *tgc = (TRANSMIT_GAMEHOUSE_CANCLE *)pmyoverinfo->szbuf;
	SendToGameUdp((PCHAR)&tgc->War3CancleHouse,sizeof(TRANSMIT_GAMEHOUSE_CANCLE));
}

void CUDPNet::DealConnectHostRoom(MYOVERINFO *pmyoverinfo)
{
	TRANSMIT_CONNECT_INFO *tci = (TRANSMIT_CONNECT_INFO *)pmyoverinfo->szbuf;
	//告知服务器
	SendToServerConnectInfo(tci->HostVirtualIpAddr);
	//本地模拟建立tcp连接
	if(this->IsCreateTcp == FALSE)
	{
		CreateGameTcp();
		IsCreateTcp = TRUE;
	}
	//开始等待连接
	PostAccept();
	//连接服务器
	PostConnectServer(&this->m_VirualHostServer);
	//关联
	this->m_VirualHost.Assosocket.sock2 = this->m_VirualHostServer.sock;
	this->m_VirualHostServer.Assosocket.sock2 = this->m_VirualHost.sock;
}

BOOL CUDPNet::PostConnectServer(VIRTUALCLIENT * Virtualclient)
{
	//创建connect socket
	SOCKET sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
	{
		return FALSE;
	}

	//关联socket
	Virtualclient->sock = sock;

	//服务器地址
	sockaddr_in gameaddr;
	gameaddr.sin_family = AF_INET;
	gameaddr.sin_port = htons(SERVER_GAME_PORT);
	gameaddr.sin_addr.S_un.S_addr = inet_addr("202.118.193.68");
	if(SOCKET_ERROR == connect(sock,(PSOCKADDR)&gameaddr,sizeof(sockaddr_in)))
	{
		closesocket(sock);
		return FALSE;
	}

	//connect成功
	CreateIoCompletionPort((HANDLE)sock,this->m_comioport,(ULONG_PTR)Virtualclient,0);
	//投递recv
	PostRecv(sock);
	//关联
	Virtualclient->Assosocket.sock1 = sock;

	return TRUE;
}

BOOL CUDPNet::SendToServerConnectInfo(in_addr HostVirtualIp)
{
	BOOL IsSend;
	TRANSMIT_CLIENT_CONNECT_HOST tcch;
	tcch.Type = TRANSMIT_PROTOCOL_CONNECTINFO;
	tcch.ClientVirtualIpAddr.S_un.S_addr = this->m_VirtualIP.S_un.S_addr;
	tcch.HostVirtualIpAddr.S_un.S_addr = HostVirtualIp.S_un.S_addr;
	IsSend = PostSendto(this->m_udpserversocket,(PCHAR)&tcch,sizeof(TRANSMIT_CLIENT_CONNECT_HOST),&m_serveraddr);
	return IsSend;
}

BOOL CUDPNet::CreateGameTcp()
{
	//建立监听socket
	this->m_tcpListensocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(this->m_tcpListensocket == INVALID_SOCKET)
	{
		return FALSE;
	}

	//bind
	sockaddr_in tcpaddr;
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(CLIENT_TCP_GAME);
	tcpaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if(SOCKET_ERROR == bind(this->m_tcpListensocket,(PSOCKADDR)&tcpaddr,sizeof(sockaddr_in)))
	{
		closesocket(this->m_tcpListensocket);
		return FALSE;
	}

	//listen
	if(SOCKET_ERROR == listen(this->m_tcpListensocket,SOMAXCONN))
	{
		closesocket(this->m_tcpListensocket);
		return FALSE;
	}

	//交给完成端口管理
	CreateIoCompletionPort((HANDLE)m_tcpListensocket,m_comioport,m_tcpListensocket,0);

	return TRUE;
}

BOOL CUDPNet::PostAccept()
{
	DWORD dwBytesReceived;
	MYOVERGAMEINFO * mygameinfo = this->m_myoverlapped.NewOverGameInfoAndAddList();
	mygameinfo->type = NT_ACCEPT;
	mygameinfo->wv.hEvent = WSACreateEvent();
	this->m_VirualHost.sock = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	if(!AcceptEx(this->m_tcpListensocket,this->m_VirualHost.sock,mygameinfo->szbuf,0,sizeof(sockaddr_in)+16,sizeof(sockaddr_in)+16,&dwBytesReceived,&mygameinfo->wv))
	{
		if(WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUDPNet::PostRecv(SOCKET sock)
{
	DWORD dwFlags = false;
	MYOVERGAMEINFO *pmygameinfo = this->m_myoverlapped.NewOverGameInfoAndAddList();
	ZeroMemory(pmygameinfo,sizeof(MYOVERGAMEINFO));
	pmygameinfo->type = NT_RECV;
	pmygameinfo->wv.hEvent = WSACreateEvent();
	pmygameinfo->wb.buf = pmygameinfo->szbuf;
	pmygameinfo->wb.len = sizeof(pmygameinfo->szbuf);

	if(WSARecv(sock,&pmygameinfo->wb,1,&pmygameinfo->dwRecv,&dwFlags,&pmygameinfo->wv,NULL))
	{
		if(WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CUDPNet::DealClientConnectData(MYOVERINFO *pmyoverinfo)
{
	BOOL bRes;
	TRANSMIT_CLIENT_INFO *tci = (TRANSMIT_CLIENT_INFO *)pmyoverinfo->szbuf;
	VIRTUALCLIENT * gameclient = new VIRTUALCLIENT;
	VIRTUALCLIENT * clientserver = new VIRTUALCLIENT;
	//连接游戏
	bRes = PostConnect(gameclient);
	Sleep(50);
	//连接服务器
	bRes = PostConnectServer(clientserver);
	//发送关联信息给服务器
	SendServerAssoInfo(clientserver->sock,tci->ClientVirtualIp);
	//关联
	gameclient->Assosocket.sock2 = clientserver->sock;
	clientserver->Assosocket.sock2 = gameclient->sock;
	//管理
	this->m_lstclient.push_back(clientserver);
	this->m_lstclient.push_back(gameclient);
	//同步
	if(bRes == TRUE)
	{
		ASSOCIATE_VIRTUAL_IP_ADDR avia;
		avia.ClientVirtualIp.S_un.S_addr = tci->ClientVirtualIp.S_un.S_addr;
		if(WAIT_OBJECT_0 == WaitForSingleObject(this->m_hQueryEvent,INFINITE))
		{
			CopyMemory(m_pavia,&avia,sizeof(ASSOCIATE_VIRTUAL_IP_ADDR));
			SetEvent(this->m_hContinueEvent);
		}
	}
}

BOOL CUDPNet::PostConnect(VIRTUALCLIENT * Virtualclient)
{
	//创建socket
	SOCKET sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
	{
		return FALSE;
	}
	//关联
	Virtualclient->sock = sock;
	//地址
	sockaddr_in gameaddr;
	gameaddr.sin_family = AF_INET;
	gameaddr.sin_port = htons(GAME_PORT);
	gameaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if(SOCKET_ERROR == connect(sock,(PSOCKADDR)&gameaddr,sizeof(sockaddr_in)))
	{
		closesocket(sock);
		return FALSE;
	}
	//connect成功
	CreateIoCompletionPort((HANDLE)sock,this->m_comioport,(ULONG_PTR)Virtualclient,0);
	//投递recv
	PostRecv(sock);
	//关联
	Virtualclient->Assosocket.sock1 = sock;

	return TRUE;
}

void CUDPNet::DealGamePacket(MYOVERGAMEINFO * pmyovergame,VIRTUALCLIENT * sock,DWORD dwNumberRecv)
{
	PostSend(sock->Assosocket.sock2,pmyovergame->szbuf,dwNumberRecv);
}

BOOL CUDPNet::PostSend(SOCKET sock,char *szbuf,int buflen)
{
	DWORD dwNumberOfBytesSend;
	MYOVERGAMEINFO * pmygameinfo = this->m_myoverlapped.NewOverGameInfoAndAddList();
	pmygameinfo->type = NT_SEND;
	pmygameinfo->wb.buf = szbuf;
	pmygameinfo->wb.len = buflen;
	pmygameinfo->wv.hEvent = WSACreateEvent();

	if(SOCKET_ERROR == WSASend(sock,&pmygameinfo->wb,1,&dwNumberOfBytesSend,0,&pmygameinfo->wv,NULL))
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUDPNet::SendToLocalVirualIpAddrToGame()
{
	BOOL bIsSend;
	TRANSMIT_VIRTUAL_IP tvi;
	tvi.Type = TRANSMIT_PROTOCOL_VIRTUALIPADDR;
	tvi.VirtualIpAddr.S_un.S_addr = this->m_VirtualIP.S_un.S_addr;
	bIsSend = SendToGameUdp((PCHAR)&tvi,sizeof(TRANSMIT_VIRTUAL_IP));
	return bIsSend;
}

BOOL CUDPNet::SendServerAssoInfo(SOCKET sock,in_addr ClientVirtualIpAddr)
{
	BOOL bIsSend;
	TRANSMIT_GAME_INFO tgi;
	tgi.Type = TRANSMIT_PROTOCOL_GAMEINFO;
	tgi.ConType = CC_CLIENT;
	tgi.ClientAddr.S_un.S_addr = ClientVirtualIpAddr.S_un.S_addr;
	tgi.ServerAddr.S_un.S_addr = this->m_VirtualIP.S_un.S_addr;
	bIsSend = PostSend(sock,(PCHAR)&tgi,sizeof(TRANSMIT_GAME_INFO));
	return bIsSend;
}
