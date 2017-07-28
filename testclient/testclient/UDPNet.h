#pragma once
#include "GlobalInformation.h"
#include "WarIIIUDPPacket.h"
#include "MyOverlapped.h"
#include <list>
#include <WinSock2.h>
#include <Mswsock.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")
using namespace std;

class CUDPNet
{
public:
	CUDPNet(void);
	~CUDPNet(void);
public:
	HANDLE m_hQueryEvent;
	HANDLE m_hContinueEvent;
	HANDLE m_hMap;
	ASSOCIATE_VIRTUAL_IP_ADDR * m_pavia;
public:
	in_addr m_VirtualIP;
public:
	SOCKET m_udpserversocket;
	SOCKET m_udpgamesocket;
	SOCKET m_tcpListensocket;
	HANDLE m_comioport;
	sockaddr_in m_serveraddr;
	CMyOverlapped m_myoverlapped;
	BOOL CreateGameUdpSocket();
	BOOL CreateGameTcp();
	BOOL IsCreateTcp;
public:
	BOOL PostRecvfrom(SOCKET sock);
	BOOL PostSendto(SOCKET sock,char *szbuf,int buflen,sockaddr_in *sendto);
	BOOL PostConnect(VIRTUALCLIENT * Virtualclient);
	BOOL PostAccept();
	BOOL PostRecv(SOCKET sock);
	BOOL PostSend(SOCKET sock,char *szbuf,int buflen);
	BOOL PostConnectServer(VIRTUALCLIENT * Virtualclient);
public:
	list<HANDLE> m_lstthreadhandle;
	VIRTUALCLIENT m_VirualHost;
	VIRTUALCLIENT m_VirualHostServer;
	list<VIRTUALCLIENT *> m_lstclient;
	BOOL IsHostRoom;
public:
	BOOL InitialUDP();
	void UnInitial();
	static unsigned __stdcall ThreadComIOPortWork(void * lpvoid);
public:
	void DeadUDPData(MYOVERINFO *pmyoverinfo);
	void DealGameUDPData(MYOVERINFO *pmyoverinfo);
	void DealTransmitData(MYOVERINFO *pmyoverinfo);
	void DealHostMapInfo(MYOVERINFO *pmyoverinfo);
	void DealPeopleChange(MYOVERINFO *pmyoverinfo);
	void DealHostCancle(MYOVERINFO *pmyoverinfo);
	void DealConnectHostRoom(MYOVERINFO *pmyoverinfo);
	void DealClientConnectData(MYOVERINFO *pmyoverinfo);
	void DealTcpBreakUp();
	void DealGamePacket(MYOVERGAMEINFO * pmyovergame,VIRTUALCLIENT * sock,DWORD dwNumberRecv);
public:
	BOOL SendToServerUDPPacket(WAR3_UDP_HEAD_PACKET * szbuf);
	BOOL SendToUserLoginInfo(PCHAR szName,PCHAR szPassword);
	BOOL SendToUserQuitInfo();
	BOOL SendToServerConnectInfo(in_addr HostVirtualIp);
	BOOL SendToLocalVirualIpAddrToGame();
	BOOL SendServerAssoInfo(SOCKET sock,in_addr ClientVirtualIpAddr);
public:
	BOOL SendToGameUdp(PCHAR szbuf,UINT ubuflen);
};

