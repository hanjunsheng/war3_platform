#ifndef UDPNET_H__
#define UDPNET_H__

#pragma once
#include <WinSock2.h>
#include <Mswsock.h>
#include "MyOverlapped.h"
#include <iostream>
#include <process.h>
#include <list>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")
using namespace std;

class CUDPNet
{
public:
	CUDPNet(void);
	~CUDPNet(void);
//管理
public:
	list<PERSON_INFO *> m_lstPersonInfo;
	list<PERSON_INFO *> m_lstHostRoom;
	list<ASSOCIATE_SOCKET *> m_GameSock;
	list<TEMP_STORE_GAME_PACKET *> m_TempGamePack;
	CRITICAL_SECTION m_connnectlock;
	void DeleteAllPersonInfo();
public:
	SOCKET m_udpsocket;
	SOCKET m_tcpsocket;
	HANDLE m_comioport;
	CMyOverlapped m_myoverlapped;
	CRITICAL_SECTION m_cs;
public:
	BOOL PostAccept();
	BOOL PostRecv(SOCKET sock);
	BOOL PostSend(SOCKET sock,char *szbuf,int buflen);
	BOOL PostRecvfrom(SOCKET sock);
	BOOL PostSendto(SOCKET sock,char *szbuf,int buflen,sockaddr_in *sendto);
	list<HANDLE> m_lstthreadhandle;
public:
	BOOL InitialUDP();
	void UnInitial();
	BOOL CreateTCPInitial();
	static unsigned __stdcall ThreadComIOPortWork( void * lpvoid);
public:
	void DealUDPData(MYOVERINFO *pmyoverinfo);
	void AnalysisGameUdpData(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf);
	void AnalysisUserLoginData(MYOVERINFO *pmyoverinfo);
	void AnalysisGameQueryData(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf);
	void AnalysisGameCreateRoom(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf);
	void AnalysisGameMapInfo(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf);
	void AnalysisUserQuitData(MYOVERINFO *pmyoverinfo);
	void AnalysisGamePeopleChange(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf);
	void AnalysisGameCancleHost(MYOVERINFO *pmyoverinfo,WAR3_UDP_HEAD_PACKET *szbuf);
	void AnalysisConnectData(MYOVERINFO *pmyoverinfo);
	void DealGameTcpConnect(MYOVERGAMEINFO *pmyoverinfo);
	void DealGameTcpRecv(ASSOCIATE_SOCKET * assosock,MYOVERGAMEINFO *pmyoverinfo,DWORD dwNumberOfRecv);
//虚拟IP
public:
	CHAR m_VirtualIP[MAX_VIRTUAL_IP_NUM];
	void DistributeVirtualIPAddress(PERSON_INFO *pi);
//房间
public:
	CHAR m_RoomNum[MAX_CREATE_ROOM_NUM];
	void DistributeGameRoomNum(PERSON_INFO *pi);
//通信
public:
	BOOL SendToClientVirtualIP(PERSON_INFO *pi);
	BOOL SendToClientAllRoomInfo(PERSON_INFO *pi,MYOVERINFO *pmyoverinfo);
	BOOL SendToClientPeopleChange(PERSON_INFO *pi,WAR3_UDP_HEAD_PACKET *szbuf);
	BOOL SendToClientCancleHost(PERSON_INFO *pi,WAR3_UDP_HEAD_PACKET *szbuf);
	BOOL SendToClientHostConnectInfo(PERSON_INFO *pi,in_addr ClientVirtualIp);
};

#endif