#ifndef GLOBALINFORMATION_H__
#define GLOBALINFORMATION_H__

#include <WinSock2.h>
#include "WarIIIUDPPacket.h"

//客户端端口
#define CLIENT_PORT 4200
//服务器端口
#define SERVER_PORT 3200
//服务器游戏
#define SERVER_GAME_PORT 3201
//默认缓冲区大小
#define DEAFAULT_BUFFER_SIZE 1024
//登陆用户名的最大长度
#define MAX_USER_NAME 20
//游戏地图 MAX SIZE
#define GAME_UDP_MAP_BUFFER	300
//查询包大小
#define GAME_UDP_QUERY_SIZE	16
//最大的虚拟IP个数
#define MAX_VIRTUAL_IP_NUM 200
//最大IP字符串长度
#define MAX_IP_STR_LENGTH 20
//最大房间数
#define MAX_CREATE_ROOM_NUM 200
//最大连接人数
#define MAX_CONNECT_NUM 20

enum CONNECTSTATE{CC_CLIENT,CC_HOST};
enum ROOMNUMISUSED{RM_NOUSING,RM_USED};
enum VIRTUALIPISUSED{VI_NOUSING,VI_UESD};
enum NETTYPE{NT_RECVFROM,NT_SENDTO,NT_ACCEPT,NT_RECV,NT_SEND};
enum PERSON_STATES{PS_INITIAL,PS_SELECTROOM,PS_GAMING};

typedef struct _TEMP_STORE_GAME_PACKET
{
	DWORD dwNumOfPacket;
	BYTE szbuf[DEAFAULT_BUFFER_SIZE];
}TEMP_STORE_GAME_PACKET;

typedef struct _ASSOCIATEIPADDR
{
	in_addr RealIpAddr;
	in_addr VirtualIpAddr;
}ASSOCIATEIPADDR;

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
	SOCKET sock;
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

//关联的socket
typedef struct _ASSOCIATE_SOCKET
{
	SOCKET sock1;
	SOCKET sock2;
	in_addr RealAddr;
	BOOL bIsGaming;
}ASSOCIATE_SOCKET;

typedef struct _PERSON_INFORMATION
{
	DWORD dwGameVersion;                                  //游戏版本<协议提取>
	CHAR szName[MAX_USER_NAME];
	GAME_VERSION_TYPE FileVersion;                        //游戏版本号
	ASSOCIATEIPADDR *lst_client[MAX_CONNECT_NUM];         //主机的客户端链表
	DWORD RoomId;                                         //房间编号
	INT ClientIndex;                                      //客户端下标
	INT VirIpIndex;                                       //虚拟ip下标
	in_addr VirtualIPAddress;                             //这是虚拟IP地址
	in_addr RealIPAddress;                                //这是真实IP地址
	PERSON_STATES PersonStates;                           //状态
	BYTE QueryPacket[GAME_UDP_QUERY_SIZE];                //查询包
	BYTE MapPacket[GAME_UDP_MAP_BUFFER];                  //地图包
	DWORD dwMapLen;				                          //地图包的长度
}PERSON_INFO;

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
	in_addr VirtualIpAddr;
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
	in_addr HostVirtualIp;                                      //主机虚拟IP地址
	ULONG uMapLen;												//地图 len
	ULONG uHouseId;											    //房间ID
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