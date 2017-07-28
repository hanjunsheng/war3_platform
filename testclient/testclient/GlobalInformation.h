#ifndef GLOBALINFORMATION_H__
#define GLOBALINFORMATION_H__

#include "WarIIIUDPPacket.h"

//WAR3端口
#define GAME_PORT 6112
//客户端端口
#define CLIENT_PORT 4200
//客户端与游戏通信端口
#define CLIENT_TCP_GAME 4201
//服务器端口
#define SERVER_PORT 3200
//服务端游戏
#define SERVER_GAME_PORT 3201
//默认缓冲区大小
#define DEAFAULT_BUFFER_SIZE 1024
//登陆用户名的最大长度
#define MAX_USER_NAME 20
//最大IP字符串长度
#define MAX_IP_STR_LENGTH 20
//event
#define MY_QUERY_EVENT_NAME "MyQueryEventName"
//continue event
#define MY_CONTINUE_ENENT_NAME "MyContinueEventName"
//map
#define MY_MAPFILE_NAME "MyMapFileName"
//一页大小
#define ONE_PAGE_SZIE 8

enum CONNECTSTATE{CC_CLIENT,CC_HOST};
enum NETTYPE{NT_RECVFROM,NT_SENDTO,NT_ACCEPT,NT_RECV,NT_SEND};

typedef struct _ASSOCIATE_SOCK
{
	SOCKET sock1;
	SOCKET sock2;
}ASSOCIATE_SOCK;

typedef struct _VIRTUALCLIENT
{
	SOCKET sock;
	ASSOCIATE_SOCK Assosocket;
}VIRTUALCLIENT;

typedef struct _ASSOCIATE_VIRTUAL_IP_ADDR
{
	in_addr ClientVirtualIp;
}ASSOCIATE_VIRTUAL_IP_ADDR;

typedef struct _MYOVERINFO
{
	WSAOVERLAPPED wv;
	WSABUF wb;
	NETTYPE type;
	CHAR szbuf[DEAFAULT_BUFFER_SIZE];
	sockaddr_in sockaddrfrom;
	INT sockaddrfromlen;
}MYOVERINFO;

//game用
typedef struct _MYOVERGAMEINFO
{
	WSAOVERLAPPED wv;
	WSABUF wb;
	NETTYPE type;
	DWORD dwRecv;
	CHAR szbuf[DEAFAULT_BUFFER_SIZE];
}MYOVERGAMEINFO;

//游戏版本 定义
enum GAME_VERSION_TYPE
{
	GAME_VERSION_UNKNOWN,
	GAME_VERSION_120E,
	GAME_VERSION_121,
	GAME_VERSION_122,
	GAME_VERSION_123,
	GAME_VERSION_124B,
	GAME_VERSION_124E,
	GAME_VERSION_125B,
	GAME_VERSION_126,
};

enum TRANSMIT_PROTOCOL_TYPE
{
	TRANSMIT_PROTOCOL_USERLOGIN,	  //这是一个用户登陆的包
	TRANSMIT_PROTOCOL_GAMEUDP,		  //这是一个游戏UDP数据包
	TRANSMIT_PROTOCOL_FILEVERSION,	  //这是游戏文件的版本号
	TRANSMIT_PROTOCOL_USERQUIT,	      //这是一个用户退出的包
	TRANSMIT_PROTOCOL_HOUSECANCEL,	  //指定的游戏主机已经被关闭
	TRANSMIT_PROTOCOL_HOUSEMAPINFO,	  //指定的游戏主机地图包信息
	TRANSMIT_PROTOCOL_CONNECTHOUSE,   //客户端请求连接指定的游戏主机
	TRANSMIT_PROTOCOL_VIRTUALIPADDR,  //分配虚拟IP地址
	TRANSMIT_PROTOCOL_PEOPLECHANGE,   //房间人数变更
	TRANSMIT_PROTOCOL_CONNECTINFO,    //通知服务器连接信息
	TRANSMIT_PROTOCOL_CLIENTINFO,     //服务器发来的客户机信息
	TRANSMIT_PROTOCOL_GAMEINFO        //tcp游戏包
};

//传输的类型
typedef struct _TRANSMIT_PROTOCOL_METHOD
{
	TRANSMIT_PROTOCOL_TYPE Protocol;
}TRANSMIT_PROTOCOL_METHOD;

//游戏的UDP数据包
typedef struct _TRANSMIT_GAME_UDP_PACKET
{
	TRANSMIT_PROTOCOL_TYPE Type;		
	WAR3_UDP_HEAD_PACKET  vBody;	//身体
}TRANSMIT_GAME_UDP_PACKET;

//用户登录对战平台服务端的封包结构
typedef struct _TRANSMIT_USER_LOGIN
{
	TRANSMIT_PROTOCOL_TYPE Type;
	CHAR	szUserName[MAX_USER_NAME];	//用户名
	CHAR    szUserPassword[MAX_USER_NAME]; //用户密码
}TRANSMIT_USER_LOGIN;

//用户退出对战平台服务端的封包结构
typedef struct _TRANSMIT_USER_QUIT
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr VirtualIPAddr;
}TRANSMIT_USER_QUIT;

//服务器分配的虚拟ip返回客户端
typedef struct _TRANSMIT_VIRTUAL_IP
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr VirtualIpAddr;
}TRANSMIT_VIRTUAL_IP;

//获取指定房间的的主机地图包的封包结构
typedef struct _TRANSMIT_GAMEHOUSE_MAPINFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIp;
	ULONG uMapLen;												//地图 len
	ULONG uHouseId;												//房间ID
	BYTE lpszMapInfo;										    //实际的地图包data
}TRANSMIT_GAMEHOUSE_MAPINFO;

//游戏房间玩家变更包
typedef struct _TRANSMIT_GAMEHOUSE_PEOPLECHANGE
{
	TRANSMIT_PROTOCOL_TYPE Type;
	WAR3_UDP_PEOPLECHANGE_PACKET War3ChangePeople;
}TRANSMIT_GAMEHOUSE_PEOPLECHANGE;

//游戏房间取消
typedef struct _TRANSMIT_GAMEHOUSE_CANCLE
{
	TRANSMIT_PROTOCOL_TYPE Type;
	WAR3_UDP_CANCELHOST_PACKET War3CancleHouse;
}TRANSMIT_GAMEHOUSE_CANCLE;

//告知客户端游戏要连接的地址
typedef struct _TRANSMIT_CONNECT_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIpAddr;
}TRANSMIT_CONNECT_INFO;

//告知客户端游戏要连接的地址
typedef struct _TRANSMIT_CLIENT_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr ClientVirtualIp;
}TRANSMIT_CLIENT_INFO;

//告知服务器谁要连接谁
typedef struct _TRANSMIT_CLIENT_CONNECT_HOST
{
	TRANSMIT_PROTOCOL_TYPE Type;
	in_addr HostVirtualIpAddr;
	in_addr ClientVirtualIpAddr;
}TRANSMIT_CLIENT_CONNECT_HOST;

//游戏数据
typedef struct _TRANSMIT_GAME_INFO
{
	TRANSMIT_PROTOCOL_TYPE Type;
	CONNECTSTATE ConType;
	in_addr ClientAddr;
	in_addr ServerAddr;
}TRANSMIT_GAME_INFO;

#endif